unsigned long intervalHeating = 0;


void ThermostatServer() {
  server.on("/pump", ThermostatPump_Web);
  server.on("/thermostat", Thermostat_Web);
}

void Thermostat_Web() {
  if (server.argName(0) == "state") {
    if (server.arg("state") == "info") {
      sensors.requestTemperatures();
      String json = "{\"temperature\":";
      json += sensors.getTempC(Thermostat_DEV);
      json += ",\"pump\":";
      json += digitalRead(THERMOSTAT_PIN) == 1 ? 0 : 1;
      json += ",\"Thermostat_MAX\":";
      json += Thermostat_MAX;
      json += ",\"Thermostat_MIN\":";
      json += Thermostat_MIN;
      json += ",\"Thermostat_EN\":";
      json += Thermostat_EN;
      json += ",\"Thermostat_Alarm_MAX\":";
      json += Thermostat_Alarm_MAX;
      json += ",\"Thermostat_Alarm_EN\":";
      json += Thermostat_Alarm_EN;
      json += ",\"Thermostat_DEV\":\"";

      String tmp = "";
      String t = "";
      for (int i = 0; i < 8; i++) {
        t = String(Thermostat_DEV[i], HEX);
        while ( t.length() < 2)  t = "0" +  t;
        tmp += t;
      }

      json += tmp;
      json += "\"}";
      server.send(200, "text/json", json);
    }

  }
  if (server.argName(0) == "sensor") {

    if (server.arg("sensor") == "list") {
      int deviceCount = sensors.getDeviceCount();
      String json = "[";
      for (int i = 0; i <= deviceCount - 1; i++) {
        DeviceAddress Address18b20;
        sensors.getAddress(Address18b20, i);
        json += "\"";
        String t = "";
        for (int i = 0; i < 8; i++) {
          t = String(Address18b20[i], HEX);
          while ( t.length() < 2)  t = "0" +  t;
          json += t;
        }
        if (i < deviceCount - 1) {
          json += "\",";
        } else {
          json += "\"";
        }

      }
      json += "]";
      server.send(200, "text/json", json);
    }
    if (server.arg("sensor") == "temperature") {
      sensors.requestTemperatures();
      String json = "{\"temperature\":";
      json += sensors.getTempC(Thermostat_DEV);
      json += "}";
      server.send(200, "text/json", json);
    }
  }
}

void ThermostatPump_Web() {
  if (server.argName(0) == "state") {
    digitalWrite(THERMOSTAT_PIN, server.arg("state").toInt() ? LOW : HIGH);
    server.send(200, "text/json", "{\"status\": \"success\"}");
  } else {
    String json = "{\"state\":";
    json += digitalRead(THERMOSTAT_PIN) == 1 ? 0 : 1;
    json += "}";
    server.send(200, "text/json", json);
  }
}

void ThermostatLoop() {
  if (millis() - intervalHeating > 10000) {
    sensors.requestTemperatures();
    float SystemTemperature = sensors.getTempC(Thermostat_DEV);
    if (Thermostat_EN) { // auto

      client.publish("Home/HS/temperature", String(SystemTemperature));
      if (SystemTemperature > Thermostat_MAX && digitalRead(THERMOSTAT_PIN)) {
        digitalWrite(THERMOSTAT_PIN, LOW);
        //client.publish("Home/HS/pump", "1");
      }
      if (SystemTemperature < Thermostat_MIN && !digitalRead(THERMOSTAT_PIN)) {
        digitalWrite(THERMOSTAT_PIN, HIGH);
        //client.publish("Home/HS/pump", "0");
      }
    }
    // alarm beep
    if (Thermostat_Alarm_EN && SystemTemperature > Thermostat_Alarm_MAX) {
      tone(THERMOSTAT_ALARM_PIN, 2750, 500);
    } else {
      noTone(THERMOSTAT_ALARM_PIN);
    }

    intervalHeating = millis();
  }
}

