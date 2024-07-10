void indata(){
     if (captivePortal()) { 
    return;
  }
  if (!validateToken()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
    if (server.hasArg("switch")){  
    String msg;   
      if(server.arg("switch")=="on"){
        settings.mqtt_en = true;
        save_config();
        msg +="<text>Передача данных включена!</text>";
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
          }
        client.setServer(ipAddress.c_str(), port.toInt());
        }else if(server.arg("switch")=="off"){
        settings.mqtt_en = false;
        save_config();
        msg +="<text>Передача данных выключена!</text>";
    } 
    server.send(200, "text/html", msg);
  }
  
    if (server.hasArg("switchpost")){  
    String top;   
      if(server.arg("switchpost")=="on"){
         settings.post_en = true;
         save_config();
         top +="<text><b>Передача данных в личный кабинет включена!</b></text><br>";
      
        }else if(server.arg("switchpost")=="off"){
        settings.post_en = false;
        save_config();
        top +="<text><b>Передача данных в личный кабинет выключена!</b></text><br>";
    } 
    server.send(200, "text/html", top);
  }
 

 if(server.hasArg("IPMQTT") && server.hasArg("PASSWORD")){  
    strncpy(settings.mqtt_serv, server.arg("IPMQTT").c_str(), MAX_STRING_LENGTH);
    settings.mqtt_user     = String(server.arg("USERNAME"));                        // Логин mqtt
    settings.mqtt_passw    = String(server.arg("PASSWORD"));                        // Пароль mqtt
    settings.mqtt_id       = String(server.arg("ID"));                              // Идентификатор mqtt 
    settings.mqtt_topic    = String(server.arg("topicname"));                       // Корень mqtt 
    //settings.mqtt_time     = server.arg("times").toInt();                           // Переодичность обмена mqtt .toInt();
    save_config();
    
    String data = "";
    data += "<text> IP адрес сервера MQTT: <b>"+String(settings.mqtt_serv)+"</b></text><br>";
    data += "<text> Логин: <b>"+settings.mqtt_user+"</b></text><br>";
    data += "<text> Идентификатор устройства: <b>"+settings.mqtt_id+"</b></text><br>";
    data += "<text> Имя корня топика: <b>"+settings.mqtt_topic+"</b></text><br>";
    //data += "<text> Периодичность обмена(с): <b>"+String(settings.mqtt_time)+"</b></text><br>";
    data += "<text><b>Данные успешно сохранены!</b></text>";
    server.send(200, "text/html", data);
  }
  
   if(server.hasArg("NEWPASSWORD")){  
       if(server.arg("NEWPASSWORD") != ""){
           settings.passwd = String(server.arg("NEWPASSWORD")); 
           save_config();
    }
   
    String data = "";
    data += "<text><b>Новый пароль успешно сохранён!</b></text>";
    server.send(200, "text/html", data);
  }
    if(server.hasArg("token")){  
       if(server.arg("token") != ""){
           strncpy(settings.post_token, server.arg("token").c_str(), MAX_STRING_LENGTH);
           save_config();
    }
   
    String data = "";
    data += "<text> Токен: <b>"+String(settings.post_token)+"</b></text><br>";
    data += "<text><b>Успешно сохранен!</b></text>";
    
    server.send(200, "text/html", data);
  }

    if(server.hasArg("times")){  
      
           settings.mqtt_time     = server.arg("times").toInt();                  // Переодичность обмена mqtt .toInt();
           save_config();
    
   
    String data = "";
    data += "<text> Периодичность обмена(мин): <b>"+String(settings.mqtt_time)+"</b></text><br>";
    data += "<text><b>Данные успешно сохранены!</b></text>";
    server.send(200, "text/html", data);
    
  } 
  if (server.hasArg("counter_config") && server.hasArg("counter_total") && server.hasArg("counter_config2") && server.hasArg("counter_total2") ){     
        settings.counter_coeff   = server.arg("counter_config").toInt();
        settings.total           = server.arg("counter_total").toFloat();
        settings.counter_coeff_2 = server.arg("counter_config2").toInt();
        settings.total_2         = server.arg("counter_total2").toFloat();
        receivedCounter0_storage = 0;
        receivedCounter1_storage = 0;
        ESP.rtcUserMemoryWrite(0, &receivedCounter0_storage, sizeof(receivedCounter0_storage));
        ESP.rtcUserMemoryWrite(sizeof(receivedCounter0_storage), &receivedCounter1_storage, sizeof(receivedCounter1_storage));
        save_config();
        String data  = "<text><b>Данные успешно сохранены!</b></text><br>";
               data += "<text><b>Установлен коэффициент счетчика 1: "+String(settings.counter_coeff)+".</b></text><br>";
               data += "<text><b>Текущие показания счетчика 1: "+String(settings.total)+".</b></text><br>";
               data += "<text><b>Установлен коэффициент счетчика 2: "+String(settings.counter_coeff_2)+".</b></text><br>";
               data += "<text><b>Текущие показания счетчика 2: "+String(settings.total_2)+".</b></text>";
     server.send(200, "text/html", data);
  }
}
