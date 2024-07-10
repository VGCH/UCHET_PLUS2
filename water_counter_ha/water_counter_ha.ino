/* Устройство для получения данных с водосчетчиков с батарейным питанием
 *  
 *  © CYBEREX Tech, 2023
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266SSDP.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "const_js.h"

// Pins
#define STATUS_LED     4               // Индикатор статуса системы / индикация импульсов
#define CONFIG_MODE    5               // Режим восстановления доступа
#define COUNTER_ACT   13
#define SLAVE_ADDRESS 18               // Адрес счетчика на шине I2C


// WEBs
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
IPAddress apIP(10, 10, 20, 1);
IPAddress netMsk(255, 255, 255, 0);
ESP8266HTTPUpdateServer httpUpdater;

// DNS server
const byte             DNS_PORT = 53;
DNSServer              dnsServer;

// Текущий статус WLAN
unsigned int status = WL_IDLE_STATUS;

// Переменные для хранения статуса сетевого подключения
boolean stat_reboot, led_stat, stat_wifi, wi_fi_st, connect, config_st ; 

// Переменные используемые для CaptivePortal
const char *myHostname  = "Cyberex_sensors";            // Имя создаваемого хоста Cyberex_sensor.local 
const char *softAP_ssid = "CYBEREX-COUNT";              // Имя создаваемой точки доступа Wi-Fi

String version_code = "WM-1.5.12.16";

// Переменная для хранения времени последней попытки сетевого подключения
unsigned long lastConnectTry = 0;
// Другие таймеры
unsigned long reboot_t = 0;
uint32_t t10s, led;
uint32_t L_1, L_2;
uint32_t receivedCounter0_storage = 0;
uint32_t receivedCounter1_storage = 0;

int count_rf = 0;

// Структура для хранения токенов сессий 
struct Token {
    String value;
    long created;
    long lifetime;
};

Token   tokens[20];                    // Длина массива структур хранения токенов 

#define TOKEN_LIFETIME 6000000         // время жизни токена в секундах

#define MAX_STRING_LENGTH 30           // Максимальное количество символов для хранения в конфигурации


// Структура для хранения конфигурации устройства
struct {
     boolean mqtt_en;
     boolean json_en;
     int     mqtt_time;
     int     statteeprom; 
     int     counter_coeff;
     float   total;
     char    mySSID[MAX_STRING_LENGTH];
     char    myPW[MAX_STRING_LENGTH];
     char    mqtt_serv[MAX_STRING_LENGTH];
     String  mqtt_user;
     String  mqtt_passw;
     String  mqtt_id;
     String  mqtt_topic;
     String  passwd;  
     int     counter_coeff_2;
     float   total_2;  
     boolean post_en;
     char    post_token[MAX_STRING_LENGTH];
  } settings;


int  CP10s, CP1m, CP5m, CP60m;                                      // Накопление импульсов счетчика 10с/1м/5м/60м 
int  bCP10s, bCP1m, bCP5m, bCP60m;                                  // Накопление импульсов счетчика 10с/1м/5м/60м буфер
float val10s, val1m, val5m, val60m;                                 // Накопленные значения расхода 10с/1м/5м/60м 
long count_total;                                                   // Общее количество импульсов счетчика

void setup() {
    EEPROM.begin(sizeof(settings));                                 // Инициализация EEPROM в соответствии с размером структуры конфигурации    
    pinMode(STATUS_LED, OUTPUT);
    ESP.rtcUserMemoryRead(0, &receivedCounter0_storage, sizeof(receivedCounter0_storage));
    ESP.rtcUserMemoryRead(sizeof(receivedCounter0_storage), &receivedCounter1_storage, sizeof(receivedCounter1_storage));
    Wire.pins(12, 14);                                              // устанавливаем пины SDA,SCL для i2c  
    Wire.begin();                                                   // Инициализируем шину I2C  SDA 12 --> 17  SCL 14 --> 19
    read_config();                                                  // Чтение конфигурации из EEPROM
    check_clean();                                                  // Провека на запуск устройства после первичной прошивки
     if(String(settings.passwd) == ""){   
        settings.passwd = "admin";                                  // Если  переменная settings.passwd пуста, то назначаем пароль по умолчанию
     }             
     WiFi.mode(WIFI_STA);                                           // Выбираем режим клиента для сетевого подключения по Wi-Fi  
     WiFi.hostname("Water-Meter [CYBEREX TECH]");                   // Указываеем имя хоста, который будет отображаться в Wi-Fi роутере, при подключении устройства
     if(WiFi.status() != WL_CONNECTED) {                            // Инициализируем подключение, если устройство ещё не подключено к сети
           WiFi.begin(settings.mySSID, settings.myPW);
      }

     for(int x = 0; x < 60; x ++){                                  // Цикл ожидания подключения к сети (30 сек)
          if (WiFi.status() == WL_CONNECTED){ 
                digitalWrite(STATUS_LED, LOW);
                stat_wifi = true;
                break;
           }
               digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));     // Мигаем светодиодом в процессе ожидания подключения
               delay(500); 
      
          }

     if(WiFi.status() != WL_CONNECTED) {                             // Если подключение не удалось, то запускаем режим точки доступа Wi-Fi для конфигурации устройства
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAPConfig(apIP, apIP, netMsk);
            WiFi.softAP(softAP_ssid);
            digitalWrite(STATUS_LED,  HIGH);
            stat_wifi = false;
        }
        
        delay(2000);
        if(digitalRead(CONFIG_MODE)){
           config_st = true;
        // Запускаем DNS сервер
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", apIP);
        // WEB страницы
        server.on("/", page_process);
        server.on("/login", handleLogin);
        server.on("/script.js", reboot_devsend);
        server.on("/style.css", css);
        server.on("/index.html", HTTP_GET, [](){
        server.send(200, "text/plain", "IoT Radiation Sensor"); });
        server.on("/description.xml", HTTP_GET, [](){SSDP.schema(server.client());});
        server.on("/inline", []() {
        server.send(200, "text/plain", "this works without need of authentification");
        });
        server.onNotFound(handleNotFound);
        const char * headerkeys[] = {"User-Agent", "Cookie"} ;               // Здесь список заголовков для записи
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
        server.collectHeaders(headerkeys, headerkeyssize);                   // Запускаем на сервере отслеживание заголовков 
        server.begin();
        connect = strlen(settings.mySSID) > 0;                               // Статус подключения к Wi-Fi сети, если есть имя сети
        SSDP_init();
        }else{
              digitalWrite(STATUS_LED,  HIGH);
              if(stat_wifi){
                 read_count();
                 post_send();
              }
              digitalWrite(STATUS_LED,  LOW);
              delay(5000);
              ESP.deepSleep((uint32_t)(settings.mqtt_time * 60e6));
        }
       
}

void loop() {
        portals();
        dnsServer.processNextRequest();
        server.handleClient();  
        led_ind();
        timers();
        reboot_dev_delay();
        //MQTT_send();
        if(millis() > 900000){
            ESP.deepSleep((uint32_t)(settings.mqtt_time * 60e6));
        }
}

void reboot_devsend(){
   server.send(200, "text", reb_d);
}
