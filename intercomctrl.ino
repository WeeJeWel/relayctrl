#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#define RELAY_TIMEOUT 500

int PIN_RELAY_1 = D2;
int PIN_RELAY_2 = D1;

ESP8266WebServer server(80);
WiFiManager wifiManager;

void setup() {
  
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  relay1Off();
  relay2Off();

  WiFi.hostname("IntercomCtrl");
  wifi_station_set_hostname("IntercomCtrl");
  
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("IntercomCtrl");
  
  if(WiFi.status() != WL_CONNECTED) {
    ESP.restart();
    return;
  }
  
  ArduinoOTA.setPort(80);
  ArduinoOTA.setHostname("IntercomCtrl");
  ArduinoOTA.begin();
  
  if (MDNS.begin("IntercomCtrl")) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("intercomctrl", "tcp", 80);
  }

  server.on("/relay1", HTTP_GET, _onHttpRelay1);
  server.on("/relay2", HTTP_GET, _onHttpRelay2);
  server.begin();
  
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}

void _onHttpRelay1() {
  toggleRelay1();
  server.send(200);
}

void _onHttpRelay2() {
  toggleRelay2();
  server.send(200);
}

void toggleRelay1() {
  relay1On();
  delay(RELAY_TIMEOUT);
  relay1Off();
}

void toggleRelay2() {
  relay2On();
  delay(RELAY_TIMEOUT);
  relay2Off();
}

void relay1On() {
  digitalWrite(PIN_RELAY_1, LOW);
}

void relay1Off() {
  digitalWrite(PIN_RELAY_1, HIGH);
}

void relay2On() {
  digitalWrite(PIN_RELAY_2, LOW);
}

void relay2Off() {
  digitalWrite(PIN_RELAY_2, HIGH);
}

