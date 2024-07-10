void timers(){
   unsigned long currentMillis = millis();  
   if(settings.counter_coeff == 0){
      settings.counter_coeff = 1;
   }
   
     float cf = (float)(1.00/settings.counter_coeff);
     if((currentMillis - t10s) > 10000 || currentMillis < t10s){  // Событие срабатывающее каждые 10 сек 
             val10s  = bCP10s*cf*1000*360;
             t10s    = currentMillis;
             CP10s   = bCP10s;
             bCP10s  = 0;
             Wire.requestFrom(SLAVE_ADDRESS, 9);                  // Запрос данных у ведущего устройства
             digitalWrite(COUNTER_ACT, LOW);
             delay(1);
             digitalWrite(COUNTER_ACT, LOW);
             while (Wire.available() < 9) {}                      // Ожидание доставки всех данных
                  receiveData();                                  // Получение данных счетчиков
             }      
  
}
void read_count(){
             Wire.requestFrom(SLAVE_ADDRESS, 9);                  // Запрос данных у ведущего устройства
             while (Wire.available() < 9) {}                      // Ожидание доставки всех данных
                  receiveData();                                  // Получение данных счетчиков    
}

void led_ind(){
   unsigned long currentMillis = millis();
   if((currentMillis - led) > 500 || currentMillis < led){
      led = currentMillis;
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
   }
       
}

void receiveData() {
  byte byteData[8];                                              // Массив для хранения принятых байтов
  Wire.readBytes(byteData, 9);                                   // Чтение данных из шины I2C
  uint32_t receivedCounter0 = ((uint32_t)byteData[0] << 24) | ((uint32_t)byteData[1] << 16) | ((uint32_t)byteData[2] << 8) | byteData[3]; // Собираем данные счетчика 0
  uint32_t receivedCounter1 = ((uint32_t)byteData[4] << 24) | ((uint32_t)byteData[5] << 16) | ((uint32_t)byteData[6] << 8) | byteData[7]; // Собираем данные счетчика 1
  uint32_t c0 = receivedCounter0 - receivedCounter0_storage;
  uint32_t c1 = receivedCounter1 - receivedCounter1_storage;
  if(receivedCounter1 != 4294967295 && receivedCounter0 != 4294967295){
              if(c0 > 429496652 && c1 > 429496652){
                    L_1 = 0;
                    L_2 = 0;
                  }else{
                    L_1 = c0;
                    L_2 = c1;
              }
              receivedCounter0_storage = receivedCounter0;
              receivedCounter1_storage = receivedCounter1;
              ESP.rtcUserMemoryWrite(0, &receivedCounter0_storage, sizeof(receivedCounter0_storage));
              ESP.rtcUserMemoryWrite(sizeof(receivedCounter0_storage), &receivedCounter1_storage, sizeof(receivedCounter1_storage));
              if(!config_st){
                  MQTT_send_data("json", JSON_DATA());
              }
    }
}
