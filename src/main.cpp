#include <Arduino.h>
#include <WiFi/WiFi_Network.h>
#include <WebService/WebService.h>
#include <Pins/Pins.h>
#include <RESTAPI/configuration.h>
#include <esp_task_wdt.h>

  void setup() {
  
  //Watchdog
  esp_task_wdt_init(45, true); //enable panic so ESP32 restarts
	esp_task_wdt_add(NULL); //add current thread to WDT watch

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

  //Watchdog reset  
  esp_task_wdt_reset();

}
