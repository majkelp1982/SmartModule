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
String version="";
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

  version = toString(doc["version"]);
  type = toString(doc["type"]);
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
  sensorsReset();
  String response = "REPS_OK";
  for (JsonObject sensor : doc["sensorDaoSet"].as<JsonArray>()) {
    String sensorName = toString(sensor["name"]);
    String sensorType = toString(sensor["type"]);
    if (sensorType == BME280SPI) {
      response = sensorBME280SPI(sensorName, sensor["csPin"]);
      if (response != RESP_OK)
        return response;
     }
         
  }
   Serial.print("\nsensorsParsing done");
 return RESP_OK;
}

String pinParsing() {
   Serial.print("\npinParsing");
  pinsReset();
  for (JsonObject pinDao : doc["pinDaoSet"].as<JsonArray>()) {
    int pinNumber = pinDao["pinNumber"];
    String pinMode = toString(pinDao["mode"]);
    String pinStandby = toString(pinDao["standby"]);
    int pinLatchTime = pinDao["latchTime"];

    //data validation
    if (pinNumber == 0)
      return RESP_PIN_NO_MISSING;
    
    if ((pinMode == NULL) || (pinMode == ""))
      return RESP_PIN_MODE_MISSING;

    if ((pinStandby == NULL) || (pinStandby == ""))
      return RESP_PIN_STANDBY_MISSING;

    if (pinLatchTime == 0)
      return RESP_PIN_LATCHTIME_MISSING;
 
    //Set pin
    String response = setPinMap(pinNumber, pinMode, pinStandby, pinLatchTime);
    if (response != RESP_OK)
      return response;  
  }

    Serial.print("\npinParsing done");
   return RESP_OK;
}

String validation() {
     Serial.print("\nvalidation\n");
  if (doc["type"].isNull()) {
      Serial.println(RESP_TYPE_MISSING);
      return RESP_TYPE_MISSING;
  }

  uint16_t configType = getHashedType(toString(doc["type"]));
  if (typeHash!=0 && typeHash!=configType && typeLocked) {
      Serial.println("\nWrong module type. Modul type is "+type+ ". Configuration is for "+configType+". To change \"typeLock\" need to be unlocked.");
      return RESP_TYPE_INCORRECT;
  }

  if (doc["version"].isNull()) {
      Serial.println(RESP_VERSION_MISSING);
      return RESP_VERSION_MISSING;
  }

  if (doc["pinDaoSet"].isNull())  {
      Serial.println(RESP_PINS_MISSING);
      return RESP_PINS_MISSING;
  }
  
  if (doc["sensorDaoSet"].isNull()) {
    Serial.println(RESP_SENSORS_MISSING);
    return RESP_SENSORS_MISSING;
  }

     Serial.print("\nvalidation done");
  //All OK
  setTypeLocked(true); 
  return RESP_OK;
}

String getVersion() {
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

void isConfigurationReady() {
  if (version == "")
    digitalWrite(BUILTIN_LED, blink1Hz());
  else digitalWrite(BUILTIN_LED, HIGH);
}