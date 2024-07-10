// Функция отправки данных по протоколу MQTT

void MQTT_send_data(String topic, String data){
          String top  = settings.mqtt_topic;
                 top +="/"+ topic;
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
          }
       
  if(settings.mqtt_en && WiFi.status() == WL_CONNECTED){
      client.setServer(ipAddress.c_str(), port.toInt());
      client.loop();
           if(client.connected()){
          count_rf = 0;
                 send_mqtt(top, data);
           }else{
              count_rf++;
              if (client.connect(settings.mqtt_id.c_str(), settings.mqtt_user.c_str(), settings.mqtt_passw.c_str())){           
                    send_mqtt(top, data);
                }else{
                  if(count_rf > 2){
                     WiFi.disconnect();
                     WiFi.begin(settings.mySSID, settings.myPW);
                     count_rf = 0;
                }
            }
        }
     }
}

void send_mqtt(String top, String data){
          // Анонсируем датчики для Home Assistant [auto-discovery ]
          // Анонс счетчика 1
          String Playload_1 = "{\"device_class\": \"water\", \"state_class\": \"total\", \"name\": \"Счетчик воды (ХВС)\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"m³\", \"value_template\": \"{{value_json.Count_1}}\" }";
          client.publish("homeassistant/sensor/wc_0/config", Playload_1.c_str(), true);
          // Анонс счетчика 2
          String Playload_2 = "{\"device_class\": \"water\", \"state_class\": \"total\",  \"name\": \"Счетчик воды (ГВС)\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"m³\", \"value_template\": \"{{value_json.Count_2}}\" }";
          client.publish("homeassistant/sensor/wc_1/config", Playload_2.c_str(), true);
          // Анонс интервального 1
          String Playload_3 = "{\"device_class\": \"water\", \"name\": \"Интервал (ХВС)\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"L\", \"value_template\": \"{{value_json.L_Count_1}}\" }";
          client.publish("homeassistant/sensor/wc_0_i/config", Playload_3.c_str(), true);
          // Анонс интервального 2
          String Playload_4 = "{\"device_class\": \"water\", \"name\": \"Интервал (ГВС)\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"L\", \"value_template\": \"{{value_json.L_Count_2}}\" }";
          client.publish("homeassistant/sensor/wc_1_i/config", Playload_4.c_str(), true);
          String Playload_5 = "{\"device_class\": \"battery\", \"name\": \"Напряжение батареи\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"V\", \"value_template\": \"{{value_json.Voltage}}\" }";
          client.publish("homeassistant/sensor/wc_bat/config", Playload_5.c_str(), true);
          
         // Отправляем данные 
         client.publish(top.c_str(), data.c_str());
}

String MQTT_status(){
    String response_message = "";
    if(client.connected()){
       response_message = "подключен";
      }else{
       response_message = "отключен";
    }
    return response_message;
} 
