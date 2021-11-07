#include <Arduino.h>
#include <ArduinoJson.h>


#define FIRMWARE              "20211107.01"

String configuration(String json);
String getVersion();
String getFirmware();
String getType();
bool isTypeLocked();
void setTypeLocked(bool value);
uint16_t getTypeHash();
void isConfigurationReady();