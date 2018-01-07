#include <ESP8266WiFi.h>          //servers://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include <ESP8266serverUpdateServer.h>
#include <WiFiManager.h>         //servers://github.com/tzapu/WiFiManager
#include <FS.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <time.h>
#include <stdio.h>

String M_Server;
int M_Port;
String M_User;
String M_Password;

#define DHT_PIN 4
#define DS18B20_PIN 5
#define THERMOSTAT_PIN 12
#define THERMOSTAT_ALARM_PIN 14

String Thermostat_ID;
DeviceAddress Thermostat_DEV;
int Thermostat_MAX;
int Thermostat_MIN;
bool Thermostat_EN;
int Thermostat_Alarm_MAX;
bool Thermostat_Alarm_EN;

//ESP8266serverUpdateServer serverUpdater;
// Web интерфейс для устройства
ESP8266WebServer server(80);
WiFiClient wclient;
PubSubClient client(wclient);
File fsUploadFile;

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT11);

void mqttConnect() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {

      if (client.connect(MQTT::Connect(WiFi.macAddress()).set_auth(M_User, M_Password))) {

        Serial.println("Connected to MQTT server ");

        String buf = "";
        for (uint8_t i = 0; i < 8; i++) {
          if (Thermostat_DEV[i] < 16) buf += "0";  // адрес датчика
          buf += String(Thermostat_DEV[i], HEX);
        }
        client.publish("Home/HS/ID", buf);
        client.publish("Home/HS/max", String(Thermostat_MAX));
        client.publish("Home/HS/min", String(Thermostat_MIN));
        client.publish("Home/HS/en", String(Thermostat_EN));
        client.publish("Home/HS/Alarm/max", String( Thermostat_Alarm_MAX));
        client.publish("Home/HS/Alarm/en", String( Thermostat_Alarm_EN));
        client.publish("Home/HS/pump", String(digitalRead(THERMOSTAT_PIN) ? 0 : 1));
        client.subscribe("Debug");
        client.subscribe("Home/HS/#");
      } else {
        Serial.println("Could not connect to MQTT server");
      }
    }
  }

  String Sys = WiFi.macAddress();
  Sys += " -- ";
  Sys += WiFi.localIP().toString();
  Sys += " -- (";
  Sys += WiFi.RSSI();
  Sys += ")";
  client.publish("Info", Sys);

}

void setup() {
  // Настраиваем вывод отладки
  Serial.begin(115200);

  //Включаем WiFiManager
  WiFiManager wifiManager;

  //Если не удалось подключиться клиентом запускаем режим AP
  // доступ к настройкам по адресу server://192.168.4.1
  wifiManager.autoConnect("AutoConnectAP");

  //если подключение к точке доступа произошло сообщаем
  Serial.println("connected...yeey :)");



  FS_init();
  loadConfig();



  pinMode(THERMOSTAT_PIN, OUTPUT);
  digitalWrite(THERMOSTAT_PIN, HIGH);
  pinMode(THERMOSTAT_ALARM_PIN, OUTPUT);

  client.set_server(M_Server, M_Port);
  client.set_callback(callback);

  sensors.begin();
  sensors.setResolution(12);
  dht.begin();
  //WiFi.hostname(Hostname);

  server_init();


}


unsigned long timer = 0;

void refreshData() {
  if (millis() - timer > 30000) {
    DeviceAddress tempDeviceAddress;

    int deviceCount = sensors.getDeviceCount();  // узнаем количество подключенных градусников
    sensors.requestTemperatures();

    client.publish("Home/DHT1/t", String(dht.readTemperature()));
    client.publish("Home/DHT1/h", String(dht.readHumidity()));

    String buf;
    for (int i = 0; i < deviceCount; i++)  { //перечисляем датчики и их показания
      sensors.getAddress(tempDeviceAddress, i);

      buf = "";
      for (uint8_t i = 0; i < 8; i++) {
        if (tempDeviceAddress[i] < 16) buf += "0";  // адрес датчика
        buf += String(tempDeviceAddress[i], HEX);
      }
      client.publish("Home/DS/" + buf, String(sensors.getTempCByIndex(i)));
    }

    timer = millis();
  }
}

void callback(const MQTT::Publish& pub)
{
  String payload = pub.payload_string();
  String topic = pub.topic();

  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.println(payload); // выводим в сериал порт значение полученных данных
  if (topic == "Home/HS/max") {
    Thermostat_MAX = payload.toInt();
    saveConfig();
  }
  if (topic == "Home/HS/min") {
    Thermostat_MIN = payload.toInt();
    saveConfig();
  }
  if (topic == "Home/HS/en") {
    Thermostat_EN = payload.toInt();
    saveConfig();
  }
  if (topic == "Home/HS/Alarm/max") {
    Thermostat_Alarm_MAX = payload.toInt();
    saveConfig();
  }
  if (topic == "Home/HS/Alarm/en") {
    Thermostat_Alarm_EN = payload.toInt();
    saveConfig();
  }
  if (topic == "Home/HS/pump") {
    digitalWrite(THERMOSTAT_PIN, payload.toInt() ? 0 : 1);
  }

  //  if(topic == "Debug" && payload == "GetNames"){
  //      for (int i = 0; i < PORT; i++) {
  //    client.publish(String("IO/"+GpioTopics[i]+"/name"), GpioDescription[i]);
  //      }
  //  }

}

void loop() {
  server.handleClient();
  delay(1);
  if (client.connected()) {
    client.loop();
    refreshData();
  } else {
    mqttConnect();
  }

  float SystemTemperature = sensors.getTempC(Thermostat_DEV);
  if (Thermostat_EN) { // auto
    
  client.publish("Home/HS/temperature", String(SystemTemperature));
    if (SystemTemperature > Thermostat_MAX && digitalRead(THERMOSTAT_PIN)) {
      digitalWrite(THERMOSTAT_PIN, LOW);
    }
    if (SystemTemperature < Thermostat_MIN && !digitalRead(THERMOSTAT_PIN)) {
      digitalWrite(THERMOSTAT_PIN, HIGH);
    }
  }
  // alarm beep
  if (Thermostat_Alarm_EN && SystemTemperature > Thermostat_Alarm_MAX) {
    tone(THERMOSTAT_ALARM_PIN, 2750, 500);
  } else {
    noTone(THERMOSTAT_ALARM_PIN);
  }

}
