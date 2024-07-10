#include <Wire.h>

int32_t  counter0 = 0;
int32_t  counter1 = 0;
uint16_t data = 0;
uint16_t data_t;

void setup() {
  pinMode(4, INPUT_PULLUP);                  // Пин счетчика 1
  pinMode(5, INPUT_PULLUP);                  // Пин счетчика 2


  attachInterrupt(0, countPulses0, FALLING); // Прерывание на INT0 при падении сигнала
  attachInterrupt(1, countPulses1, FALLING); // Прерывание на INT1 при падении сигнала

  Wire.begin(18);                            // Устанавливаем адрес устройства I2C
  //PB5: SDA (Serial Data)  - 12
  //PB7: SCL (Serial Clock) - 14
  Wire.onRequest(sendCounters);              // Устанавливаем функцию обработки запроса на передачу данных
}

void loop() {
     sender();                               // Это пустая функция в точке входа, чтобы контроллер работал, иначе не работает
}

void countPulses0() {
  counter0++;
}

void countPulses1() {
  counter1++;
}

void sender(){
  if((millis() - data_t) > data*3600000 && data >= 1 ){    // Событие которое не сработает никогда 
          data_t    = millis();
          digitalWrite(10, HIGH);
          while((data_t + 1000) > millis()){}
          digitalWrite(10, LOW);
        }

}

void sendCounters() {
    byte byteData[8];                                      // массив для хранения байтов
    byteData[0] = (counter0 >> 24) & 0xFF;                 // старший байт счетчика 0
    byteData[1] = (counter0 >> 16) & 0xFF;
    byteData[2] = (counter0 >> 8) & 0xFF;
    byteData[3] = counter0 & 0xFF;                         // младший байт счетчика 0

    byteData[4] = (counter1 >> 24) & 0xFF;                 // старший байт счетчика 1
    byteData[5] = (counter1 >> 16) & 0xFF;
    byteData[6] = (counter1 >> 8) & 0xFF;
    byteData[7] = counter1 & 0xFF;                         // младший байт счетчика 1


    Wire.write(byteData, 8);                               // Отправляем данные по шине I2C
   
   }
