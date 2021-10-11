#include <Arduino.h>
#include <WiFi/WiFi_Network.h>
#include <WebService/WebService.h>
#include <Pins/Pins.h>
#include <RESTAPI/configuration.h>

  void setup() {
  Serial.begin(9600);
  Serial.println("Serial started");
  WiFi_network();
  Serial.println("WiFi connected");
  webService_setup();
  Serial.println("WebServer strted");
}

void loop() {
  WiFi_conectionCheck();
  isConfigurationReady();
  webService_run();
  scanPins();
}
