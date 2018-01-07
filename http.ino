void server_init(void) {
  server.on("/restart", restart);
  server.on("/system_info", system_info_handler);
  server.on("/available_networks", available_networks_handler);
 server.on("/io", gpio_handler);
 server.on("/mqtt", config_mqtt_handler);
  update();
  server.begin();
}

void config_mqtt_handler() {
  if (server.argName(0) == "server" && server.argName(1) == "port" && server.argName(2) == "user" && server.argName(3) == "password") {
    M_Server = server.arg("server");
    M_Port = server.arg("port").toInt();
    M_User = server.arg("user");
    M_Password = server.arg("password");
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
  
  //server.send(404, "text/plain", "ERR"); // отправляем ответ о выполнении
}

void gpio_handler() {
  if (server.args() == 0) {
      String json = "[";
     for(int i=0; i<4;i++){
       json += "[";
      //json += GpioLevel[digitalRead(GpioList[i])];
      json += ",\"";
     // json += GpioDescription[i];
      
      if(i != 3){
      json += "\"],";
      }else {
          json += "\"]";
      }
     }
      
      json += "]";
  server.send(200, "text/json", json);
   
  }
  if (server.argName(0) == "set" && server.argName(1) == "val") {
     //digitalWrite(GpioList[server.arg("set").toInt()], GpioLevel[server.arg("val").toInt()]);
    server.send(200); // отправляем ответ о выполнении
  }
}

void available_networks_handler() {
  String json = "[";
  int n = WiFi.scanNetworks();
  if (n) {
    for (int i = 0; i < n; ++i) {
      if (i) {
        json += ",";
      }
      json += "{";
      json += "\"ssid\":\"";
      json += WiFi.SSID(i);
      json += "\",\"rssi\":";
      json += WiFi.RSSI(i);
      json += ",\"encryption\":\"";
      //      json += WiFi.encryptionType(i);
      switch (WiFi.encryptionType(i)) {
        case ENC_TYPE_NONE:
          json += "NONE";
          break;
        case ENC_TYPE_WEP:
          json += "WEP";
          break;
        case ENC_TYPE_TKIP:
          json += "TKIP";
          break;
        case ENC_TYPE_CCMP:
          json += "CCMP";
          break;
        case ENC_TYPE_AUTO:
          json += "AUTO";
          break;
        default:
          json += "?";
          break;
      }
      json += "\"}";
      delay(10);
    }
  }
  json += "]";
  server.send(200, "text/json", json);
}

void restart() {
  server.send(200, "text / plain", "Reset OK");
  ESP.restart();
}

void system_info_handler() {
  String json = "{";
  json += "\"mac\":\"";
  json += WiFi.macAddress();
  json += "\",\"ip\":\"";
  json +=  WiFi.localIP().toString();
  json += "\",\"ssid\":\"";
  json += WiFi.SSID();
  json += "\",\"rssi\":";
  json += WiFi.RSSI();
  json += ",\"uptime\":";
  json += millis();
  json += ",\"vcc\":";
  json +=  ESP.getVcc();
  json += "}";
  server.send(200, "text/json", json);
}
/*
void pwm_handler() {
  if (server.argName(0) == "add") {
    pinMode(server.arg("add").toInt(), OUTPUT);
    server.send(200); // отправляем ответ о выполнении
  }
  if (server.argName(0) == "set" && server.argName(1) == "val") {
     analogWrite(server.arg("set").toInt(), server.arg("val").toInt());
    server.send(200); // отправляем ответ о выполнении
  }
}

void servo_handler() {
  if (server.argName(0) == "deg") {
    
    myservo.write(server.arg("deg").toInt());
    server.send(200); // отправляем ответ о выполнении

  }
}

void history_handler() {
  String json = "{";
  json += "\"time\" : [";
  for (int i = 0; i < 12; i++) {
    json += "\"";
    json += GetTime(Time_h[i]);
    json += "\"";
    if (i < 11) {
      json += ",";
    }
  }
  json += "],";
  json += "\"data\" : [";
  for (int i = 0; i < 10; i++) {
    if (i) {
      json += ",[";
    } else {
      json += "[";
    }

    for (int j = 0; j < 12; j++) {
      json += History[j][i];
      if (j < 11) {
        json += ",";
      }
    }

    json += "]";
  }
  json += "]";
  json += "}";
  server.send(200, "text/json", json);
}

void available_networks_handler() {
  String json = "[";
  int n = WiFi.scanNetworks();
  if (n) {
    for (int i = 0; i < n; ++i) {
      if (i) {
        json += ",";
      }
      json += "{";
      json += "\"ssid\":\"";
      json += WiFi.SSID(i);
      json += "\",\"rssi\":";
      json += WiFi.RSSI(i);
      json += ",\"encryption\":\"";
      //      json += WiFi.encryptionType(i);
      switch (WiFi.encryptionType(i)) {
        case ENC_TYPE_NONE:
          json += "NONE";
          break;
        case ENC_TYPE_WEP:
          json += "WEP";
          break;
        case ENC_TYPE_TKIP:
          json += "TKIP";
          break;
        case ENC_TYPE_CCMP:
          json += "CCMP";
          break;
        case ENC_TYPE_AUTO:
          json += "AUTO";
          break;
        default:
          json += "?";
          break;
      }
      json += "\"}";
      delay(10);
    }
  }
  json += "]";
  server.send(200, "text/json", json);
}

void restart() {
  server.send(200, "text / plain", "Reset OK");
  ESP.restart();
}

void system_info_handler() {
  String json = "{";
  json += "\"mac\":\"";
  json += WiFi.macAddress();
  json += "\",\"ip\":\"";
  json +=  WiFi.localIP().toString();
  json += "\",\"ssid\":\"";
  json += WiFi.SSID();
  json += "\",\"rssi\":";
  json += WiFi.RSSI();
  json += ",\"uptime\":";
  json += millis();
  json += ",\"vcc\":";
  json +=  ESP.getVcc();
  json += "}";
  server.send(200, "text/json", json);
}

void hostname_config() {
  if (server.argName(0) == "name") {
    Hostname = server.arg("name");
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
    ESP.restart();
  }
}
void ds_config() {
  if (server.argName(0) == "en") {
    DS_EN = server.arg("en").toInt();
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
}
void dht_config() {
  if (server.argName(0) == "en") {
    DHT_EN = server.arg("en").toInt();
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
}
void bmp_config() {
  if (server.argName(0) == "en") {
    BMP_EN = server.arg("en").toInt();
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
}

void nm_config() {
  if (server.argName(0) == "en") {
    NM_EN = server.arg("en").toInt();
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
  if (server.argName(0) == "interval") {
    NM_INTERVAL = server.arg("interval").toInt();
    saveConfig();
    server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
}
*/
//void handle_Set_DS18b20_Pin() {
//  DS18B20_PIN = server.arg("pin").toInt(); // Получаем значение ssdp из запроса сохраняем в глобальной переменной
//  saveConfig();                 // Функция сохранения данных во Flash пока пустая
//  server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
//}
