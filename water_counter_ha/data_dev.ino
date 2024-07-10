 String millis2time(){ // преобразование миллисекунд в вид д/ч/м/с
  
         int times =(millis()/1000);
         int days =  (times/3600)/24;
         int timehour =(((times)  % 86400L) / 3600); // часы
        if ( ((times % 3600) / 60) < 10 ) {
         int timehour = 0;
               }
         int timeminuts=((times  % 3600) / 60); // минуты
         if ( (times % 60) < 10 ) {
         int timeminuts = 0;
             }
         int timeseconds=(times % 60); // секунды
       String Time= String(days)+":"+String(twoDigits(timehour))+":"+String(twoDigits(timeminuts))+":"+String(twoDigits(timeseconds));
       return Time;
     }
     
 String millis2time_minus(){ // преобразование миллисекунд в вид д/ч/м/с
  
         int times = 900 - (millis()/1000);
         int days =  (times/3600)/24;
         int timehour =(((times)  % 86400L) / 3600); // часы
        if ( ((times % 3600) / 60) < 10 ) {
         int timehour = 0;
               }
         int timeminuts=((times  % 3600) / 60); // минуты
         if ( (times % 60) < 10 ) {
         int timeminuts = 0;
             }
         int timeseconds=(times % 60); // секунды
       String Time= String(days)+":"+String(twoDigits(timehour))+":"+String(twoDigits(timeminuts))+":"+String(twoDigits(timeseconds));
       return Time;
     }

 String twoDigits(int digits){
             if(digits < 10) {
          String i = '0'+String(digits);
          return i;
         }
          else {
        return String(digits);
         }
      }

void time_work(){
   if (captivePortal()) {  
    return;
  }
  String header;
  if (validateToken()){
     float cf = (float)(((receivedCounter0_storage * settings.counter_coeff) * 0.001) + settings.total);
     float cf_2 = (float)(((receivedCounter1_storage * settings.counter_coeff_2) * 0.001) + settings.total_2);
     String outjson = "{";
     outjson += "\"sensor\":\"water\",";
     outjson += "\"time\":\""+millis2time()+"\",";
     outjson += "\"timew2\":\""+millis2time_minus()+"\",";
     outjson += "\"Count1\":\""+String(receivedCounter0_storage)+"\",";
     outjson += "\"Count2\":\""+String(receivedCounter1_storage)+"\",";
     outjson += "\"Voltage\":\""+String(analogRead(A0)*0.0049)+"\",";
     outjson += "\"Count_1\":\""+String(cf)+"\",";
     outjson += "\"Count_2\":\""+String(cf_2)+"\",";
     outjson += "\"MQTT\":\""+MQTT_status()+"\"";
     outjson += "}";
     server.send(200, "text", outjson);   
  }   
}

String JSON_DATA(){    
     float cf     = (float)(((receivedCounter0_storage * settings.counter_coeff) * 0.001) + settings.total);
     float cf_2   = (float)(((receivedCounter1_storage * settings.counter_coeff_2) * 0.001) + settings.total_2);
     float L_cf   = (float)(L_1 * settings.counter_coeff);
     float L_cf_2 = (float)(L_2 * settings.counter_coeff_2);
     String outjson = "{";
     outjson += "\"Count1\":\""+String(receivedCounter0_storage)+"\",";
     outjson += "\"Count2\":\""+String(receivedCounter1_storage)+"\",";
     outjson += "\"Voltage\":\""+String(analogRead(A0)*0.0049)+"\",";
     outjson += "\"Count_1\":\""+String(cf)+"\",";
     outjson += "\"Count_2\":\""+String(cf_2)+"\",";
     outjson += "\"L_Count_1\":\""+String(L_cf)+"\",";
     outjson += "\"L_Count_2\":\""+String(L_cf_2)+"\",";
     outjson += "\"ID\":\""+String(ESP.getChipId())+"\",";
     outjson += "\"WORKTIME\":\""+String(millis()/1000)+"\"";
     outjson += "}";
     return outjson;
}
