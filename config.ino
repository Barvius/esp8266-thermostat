String jsonConfig = "{}";
bool loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    // если файл не найден
    Serial.println("Failed to open config file");
    //  Создаем файл запиав в него аные по умолчанию
    saveConfig();
    return false;
  }
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  //  строку возьмем из глобальной переменной String jsonConfig
  JsonObject& root = jsonBuffer.parseObject(jsonConfig);
  // Теперь можно получить значения из root
  //timezone = root["timezone"];               // Так получаем число
  //_ssid = root["ssidName"].as<String>();
  //_password = root["ssidPassword"].as<String>();
  M_Server = root["M_Server"].as<String>();
  M_Port = root["M_Port"];
  M_User = root["M_User"].as<String>();
  M_Password = root["M_Password"].as<String>();
  String a = root["Thermostat_DEV"].as<String>();
  String b;
  for (int i = 0; i < 8; i++) {
    b = a[i * 2];
    b += a[i * 2 + 1];
    Serial.print(b);
    Thermostat_DEV[i] = hexToDec(b);
  } 
  Thermostat_MAX = root["Thermostat_MAX"];
  Thermostat_MIN = root["Thermostat_MIN"];
  Thermostat_EN = root["Thermostat_EN"];
  Thermostat_Alarm_MAX = root["Thermostat_Alarm_MAX"];
  Thermostat_Alarm_EN = root["Thermostat_Alarm_EN"];
  //Serial.println();
  //for (int i = 0; i < 8; i++) {
  //    if (Thermostat_DEV[i] < 16)  Serial.print("0");
  //    Serial.print(Thermostat_DEV[i], HEX);
  //  }
  //  Serial.println();
  //DeviceAddress SystemSensor = { 0x28, 0xFF, 0x77, 0xC9, 0x74, 0x16, 0x04, 0x88 };
  //  for (int i = 0; i < 8; i++) {
  //    if (SystemSensor[i] < 16)    Serial.print("0");
  //    Serial.print(SystemSensor[i], HEX);
  //  }
  //   Serial.println();
  return true;
}

unsigned long hexToDec(String hexString) {
  unsigned long ret;
  for (int i = 0, n = hexString.length(); i != n; ++i) {
    char ch = hexString[i];
    int val = 0;
    if ('0' <= ch && ch <= '9')      val = ch - '0';
    else if ('a' <= ch && ch <= 'f') val = ch - 'a' + 10;
    else if ('A' <= ch && ch <= 'F') val = ch - 'A' + 10;
    else continue; // skip non-hex characters
    ret = ret * 16 + val;
  }
  return ret;
}

// Запись данных в файл config.json
bool saveConfig() {
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;

  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  json["M_Server"] = M_Server;
  json["M_Port"] = M_Port;
  json["M_User"] = M_User;
  json["M_Password"] = M_Password;
  String tmp = "";
  String t = "";
  for (int i = 0; i < 8; i++) {
    t = String(Thermostat_DEV[i], HEX);
    while ( t.length() < 2)  t = "0" +  t;
    tmp += t;
  }
  json["Thermostat_DEV"] = tmp;
  json["Thermostat_MAX"] = Thermostat_MAX;
  json["Thermostat_MIN"] = Thermostat_MIN;
  json["Thermostat_EN"] = Thermostat_EN;
  json["Thermostat_Alarm_MAX"] = Thermostat_Alarm_MAX;
  json["Thermostat_Alarm_EN"] = Thermostat_Alarm_EN;


  // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  json.printTo(jsonConfig);
  // Открываем файл для записи
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }
  // Записываем строку json в файл
  json.printTo(configFile);
  return true;
}
