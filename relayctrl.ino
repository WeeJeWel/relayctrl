#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#define RELAY_TIMEOUT 500

int PIN_RELAY = D1;

ESP8266WebServer server(80);
WiFiManager wifiManager;

void setup() {
  
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();

  WiFi.hostname("RelayCtrl");
  wifi_station_set_hostname("RelayCtrl");
  
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("RelayCtrl");
  
  if(WiFi.status() != WL_CONNECTED) {
    ESP.restart();
    return;
  }
  
  ArduinoOTA.setPort(80);
  ArduinoOTA.setHostname("RelayCtrl");
  ArduinoOTA.begin();
  
  if (MDNS.begin("RelayCtrl")) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("RelayCtrl", "tcp", 80);
  }

  server.on("/on", HTTP_GET, _onHttpRelayOn);
  server.on("/off", HTTP_GET, _onHttpRelayOff);
  server.on("/toggle", HTTP_GET, _onHttpRelayToggle);
  server.begin();
  
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}

void _onHttpRelayOn() {
  relayOn();
  server.send(200);
}

void _onHttpRelayOff() {
  relayOff();
  server.send(200);
}

void _onHttpRelayToggle() {
  relayToggle();
  server.send(200);
}

void relayToggle() {
  relayOn();
  delay(RELAY_TIMEOUT);
  relayOff();
}

void relayOn() {
  digitalWrite(PIN_RELAY, HIGH);
}

void relayOff() {
  digitalWrite(PIN_RELAY, LOW);
}

