#include "Arduino.h"
#include <WebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <WebService/OTA.h>
#include <RESTAPI/configuration.h>
#include <Model/HTTP_resp.h>
#include <RESTAPI/service.h>
#include <WiFi/WiFi_Network.h>

void webService_setup();
void webService_run();