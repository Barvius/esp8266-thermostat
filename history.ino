float History[12][10];
time_t Time_h[12];

void push_history(int row, float val) {
  for (int i = 0; i < 12; i++) {
    History[i][row] = History[i + 1][row];
  }
  History[11][row] = val;
}

void history() { // Собственно формирование пакета и отправка.
  timeSynch(3);

  push_history(0, (float)dht.readTemperature());
  push_history(1, (float)dht.readHumidity());

  int deviceCount = sensors.getDeviceCount();  // узнаем количество подключенных градусников
  sensors.requestTemperatures();
  DeviceAddress tempDeviceAddress;
  for (int i = 0; i < deviceCount; i++)  {
    sensors.getAddress(tempDeviceAddress, i);
    push_history(2 + i, sensors.getTempCByIndex(i));
  }

  for (int i = 0; i < 12; i++) {
    Time_h[i] = Time_h[i + 1];
  }
  Time_h[11] = GetUnixTime();
}

void raw() {

  int deviceCount = sensors.getDeviceCount();  // узнаем количество подключенных градусников
  sensors.requestTemperatures();

  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");

  String json = "{";

  json += "\"dht\":{";
  json += "\"t\":";
  json += (float)dht.readTemperature();
  json += ",\"h\":";
  json += (float)dht.readHumidity();
  json += "}";

  for (int i = 0; i <= deviceCount - 1; i++) {
    DeviceAddress Address18b20;
    sensors.getAddress(Address18b20, i);

    json += ",\"ds_";

    json += i;
    json += "\":{";
    json += "\"t\":";
    json += sensors.getTempC(Address18b20);

    json += "}";
  }

  json += "}";


  server.send(200, "text/json", json);
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

