#include <RESTAPI/service.h>
#include <Model/HTTP_resp.h>
#include <Utils/Utils.h>
#include <Pins/Pins.h>
#include <Sensors/Sensors.h>
#include <RESTAPI/configuration.h>
#include <WiFi/WiFi_Network.h>

String pinAction();
String sensorsAction();
String sensorBME280(String);
String requestValidation();

StaticJsonDocument<500> docu;
StaticJsonDocument<500> respJSON;

String action(String json) {
  String resp = RESP_OK;
  docu.clear();
  respJSON.clear();
  DeserializationError error = deserializeJson(docu, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return RESP_JSON_DESERIALIZATION_FAILED;
  }

  resp = requestValidation();
  if (resp != RESP_OK)
    return resp;

  Serial.print("\nDeserialization done");
  Serial.print("\npins parsing");

  respJSON["type"] = getType();
  respJSON["version"] = getVersion();
 
  resp = pinAction();
  if (resp != RESP_OK)
    return resp;

  Serial.print("\nsensors parsing");
  resp = sensorsAction();
  if (resp != RESP_OK)
    return resp;
  String output;
  serializeJson(respJSON, output);
  return output;
}

String requestValidation() {
  String requestType = docu["type"];
  Serial.print("\nrequest type: "+requestType);
  if ((requestType == NULL) || (requestType=="")) {
    Serial.print("\n");
    Serial.print(RESP_TYPE_MISSING);
    return RESP_TYPE_MISSING;
  }


  if (requestType != getType()) {
    Serial.print("\n");
    Serial.print(RESP_TYPE_NOT_EQUAL);
    return RESP_TYPE_NOT_EQUAL;
  }


  String requestVersion = docu["version"];
  Serial.print("\nrequest version: "+requestVersion);
  if ((requestVersion == NULL) || (requestVersion=="")) {
    Serial.print("\n");
    Serial.print(RESP_VERSION_MISSING);
    return RESP_VERSION_MISSING;
  }


  if (requestVersion != getVersion()) {
    Serial.print("\n");
    Serial.print(RESP_VERSION_NOT_EQUAL);
    return RESP_VERSION_NOT_EQUAL;
  }
  return RESP_OK;
}

String pinAction() {
  String result = RESP_OK;
  JsonArray pinResponseSet = respJSON.createNestedArray("pinResponseSet");
  for (JsonObject pinCommand : docu["pinCommandSet"].as<JsonArray>()) {
    //fixme need to be extended. Now only outputs
    int pinNumber = pinCommand["pinNumber"];
    String action = toString(pinCommand["action"]);
    result = writePin(pinNumber, action);
    if (result != RESP_OK)
      return result;
  }
  return RESP_OK;
}

String sensorsAction() {
  JsonArray sensorResponseSet = respJSON.createNestedArray("sensorResponseSet");
  for (JsonObject sensorCommand : docu["sensorCommandSet"].as<JsonArray>()) {
    String sensorName = toString(sensorCommand["sensorName"]);
    String action = toString(sensorCommand["action"]);
    String response = sensorRead(sensorName);

    //sensor read not ok
    if (response[0] != '{')
      return response;

    JsonObject sensorResponse = sensorResponseSet.createNestedObject();
    sensorResponse["sensorName"]=sensorName;
    sensorResponse["response"]=response;
    
    // JsonObject sensorValues = sensorResponse.createNestedObject();
    // if (response.indexOf(BME280SPI) != -1) {
    //   int indexStart = response.indexOf("temperature")+12;
    //   int indexEnd = response.indexOf(";", indexStart);
    //   String temp = response.substring(indexStart, indexEnd);   

    //   indexStart = response.indexOf("pressure")+9;
    //   indexEnd = response.indexOf(";", indexStart);
    //   String pressure = response.substring(indexStart, indexEnd);   

    //   indexStart = response.indexOf("humidity")+9;
    //   indexEnd = response.indexOf(";", indexStart);
    //   String humidity = response.substring(indexStart, indexEnd);   

    //   sensorValues["temperature"]=temp;
    //   sensorValues["pressure"]=pressure;
    //   sensorValues["humidity"]=humidity;
    
  }
  return RESP_OK;  
}

String getDeviceParams() {
  docu.clear();
	String result;
	docu["macAddress"] = getMacAddress();
	docu["firmware"] = getFirmware();
  docu["version"] = getVersion();
  docu["moduleType"] = getType();
  docu["typeHash"] = getTypeHash();
  docu["isTypeLocked"] = isTypeLocked();
	serializeJson(docu, result);
	return result;
}