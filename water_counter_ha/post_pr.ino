void post_send(){

  if(settings.post_en && WiFi.status() == WL_CONNECTED){
        float cf     = (float)(((receivedCounter0_storage * settings.counter_coeff) * 0.001) + settings.total);
        float cf_2   = (float)(((receivedCounter1_storage * settings.counter_coeff_2) * 0.001) + settings.total_2);
        float L_cf   = (float)(L_1 * settings.counter_coeff);
        float L_cf_2 = (float)(L_2 * settings.counter_coeff_2);
       String data = "";
              data += "key=";
              data += String(settings.post_token);
              data += "&Count1=";
              data += String(receivedCounter0_storage);
              data += "&Count2=";
              data += String(receivedCounter1_storage);
              data += "&Voltage=";
              data += String(analogRead(A0)*0.0049);
              data += "&Count_1=";
              data += String(cf);
              data +="&Count_2=";
              data += String(cf_2);
              data +="&L_Count_1=";
              data += String(L_cf);
              data +="&L_Count_2=";
              data += String(L_cf_2);
              data +="&ID=";
              data += String(ESP.getChipId());
              if (espClient.connect("plus.cyberex.online", 80)) {  
                   espClient.println("POST /indata.php HTTP/1.1");
                   espClient.println("Host: plus.cyberex.online");
                   espClient.println("User-Agent: ESP8266/1.0");
                   espClient.println("Connection: close");
                   espClient.println("Content-Type: application/x-www-form-urlencoded");
                   espClient.print("Content-Length: ");
                   espClient.println(data.length());
                   espClient.println();
                   espClient.print(data);
                   delay(100);
                if (espClient.connected()) { 
                      espClient.stop();  // DISCONNECT FROM THE SERVER
                  }
              }
        }
  
}
