#ifndef WIFI_NETWORK_H_
#define WIFI_NETWORK_H_

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <RESTAPI/service.h>

#define SSID                    "Majkel"
#define PASS                    "12345678"
#define MANAGERURL              "http://192.168.0.105:9999/settings"

void waitTillConnected();
void WiFi_network();
void WiFi_conectionCheck();
String getMacAddress();
void sendStatusToModulManager();

#endif /* WIFI_NETWORK_H_ */
