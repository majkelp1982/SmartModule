#include <Arduino.h>
#include <ArduinoJson.h>

String configuration(String json);
int getVersion();
String getFirmware();
String getType();
bool isTypeLocked();
void setTypeLocked(bool value);
uint16_t getTypeHash();