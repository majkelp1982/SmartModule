#include <RESTAPI/configuration.h>
#include <ArduinoJson.h>
#include <Utils/Utils.h>
#include <Model/HTTP_resp.h>
#include <Sensors/Sensors.h>
#include <Pins/Pins.h>

#define FIRMWARE              "20211010.20"

String validation();
String pinParsing();
String sensorsParsing();
void sensorBME280(JsonPair pair);
uint16_t getHashedType(String type);

//Fields
int version=0;
String type="";
uint16_t typeHash=0;
bool typeLocked;

//Help variable
const char* tmpChar;
StaticJsonDocument<500> doc;

String configuration(String json) {
  Serial.print("\nconfiguration");

  doc.clear();
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return RESP_JSON_DESERIALIZATION_FAILED;
  }

  String response = validation();
  Serial.println(response);
 
 if (response != RESP_OK)
      return response;

  version = doc["version"].as<int>();
  type = getString(doc["type"]);
  typeHash = getHashedType(type);

  response = pinParsing();
  if (response != RESP_OK)
    return response;

  response = sensorsParsing();
  if (response != RESP_OK)
    return response;  
 
  //OK
  Serial.print("\nConfiguration done: Type: "+type+" Version: "+version);
  return RESP_OK;
}

String sensorsParsing() {
  Serial.print("\nsensorsParsing");
  JsonObject root = doc["sensors"].as<JsonObject>();
  sensorsReset();
  for (JsonPair kv : root) {
     String nameStr = getString(kv.key().c_str()); 
     String typeStr = getString(kv.value()["type"]);
     if (typeStr == BME280SPI) {
       String csPinStr = getString(kv.value()["cspin"]);
       sensorBME280SPI(nameStr, csPinStr.toInt());
     }
   }
   Serial.print("\nsensorsParsing done");
 return RESP_OK;
}

String pinParsing() {
   Serial.print("\npinParsing");
  JsonObject root = doc["pins"].as<JsonObject>();
  pinsReset();
  for (JsonPair kv : root) {
    String pinStr = getString(kv.key().c_str());
    int pinInt = pinStr.toInt();
    if (pinInt == 0)
      return RESP_PINS_NO_DESERIALIZATION;

    String modeStr = getString(kv.value()["mode"]);
    String standbyStr = getString(kv.value()["standby"]);

    String response = setPinMap(pinInt, modeStr, standbyStr);
    if (response != RESP_OK)
      return response;
  }
    Serial.print("\npinParsing done");
   return RESP_OK;
}

String validation() {
     Serial.print("\nvalidation");
  if (doc["type"].isNull()) {
      Serial.println(RESP_TYPE_MISSING);
      return RESP_TYPE_MISSING;
  }

  uint16_t configType = getHashedType(getString(doc["type"]));
  if (typeHash!=0 && typeHash!=configType && typeLocked) {
      Serial.println("\nWrong module type. Modul type is "+type+ ". Configuration is for "+configType+". To change \"typeLock\" need to be unlocked.");
      return RESP_TYPE_INCORRECT;
  }

  if (doc["version"].isNull()) {
      Serial.println(RESP_VERSION_MISSING);
      return RESP_VERSION_MISSING;
  }

  if (doc["pins"].isNull())  {
      Serial.println(RESP_PINS_MISSING);
      return RESP_PINS_MISSING;
  }
  
  if (doc["sensors"].isNull()) {
    Serial.println(RESP_SENSORS_MISSING);
    return RESP_SENSORS_MISSING;
  }

     Serial.print("\nvalidation done");
  //All OK
  setTypeLocked(true); 
  return RESP_OK;
}

int getVersion() {
    return version;
}

String getFirmware() {
  return FIRMWARE;
}

String getType() {
    return type;
}

uint16_t getTypeHash() {
  return typeHash;
}

uint16_t getHashedType(String type) {
  int value=0;
  for (int i=0; i<type.length(); i++)
    value += (int) type.charAt(i);
  return value;
}

bool isTypeLocked() {
  return typeLocked;
}

void setTypeLocked(bool value) {
  typeLocked = value;
}

bool isConfigurationReady() {
  if (version == 0)
    digitalWrite(BUILTIN_LED, blink1Hz());
  else digitalWrite(BUILTIN_LED, HIGH);
}