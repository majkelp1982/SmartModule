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

StaticJsonDocument<500> docu;

String action(String json) {
  String response;
  docu.clear();
  DeserializationError error = deserializeJson(docu, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return RESP_JSON_DESERIALIZATION_FAILED;
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

  Serial.print("\nDeserialization done");
  Serial.print("\npins parsing");
  response = pinAction();
  if (response != RESP_OK)
    return response;

 Serial.print("\nsensors parsing");
 response = sensorsAction();
  if (response != RESP_OK)
    return response;
  return RESP_OK;
}

String pinAction() {
  JsonObject root = docu["pins"].as<JsonObject>();
  String result;
  for (JsonPair kv : root) {
    String pinStr = toString(kv.key().c_str());
    String stateStr = toString(kv.value()["state"]);
    String latchStr = toString(kv.value()["latch"]);

    result = writePin(pinStr, stateStr, latchStr);
    if (result != RESP_OK)
      return result;
  }
  return RESP_OK;
}

String sensorsAction() {
  Serial.print("\n\nsensorsAction");
  String result="\"sensors\":";

  JsonArray senorsArray = docu["sensors"];
  for (JsonVariant sensorName : senorsArray) {
    String response = "";
    String nameStr = toString(sensorName.as<char*>());
    Serial.print("\nSensor name: "+nameStr);
    response = sensorRead(nameStr);
    if (response[0] != '{')
       return response;

    result.concat(response);
  }
Serial.print("\nsensorsAction done");
 return result;
}

String getDeviceParams() {
  docu.clear();
	String result;
	docu["macAddress"] = getMacAddress();
  docu["version"] = getVersion();
  docu["type"] = getType();
  docu["typeHash"] = getTypeHash();
  docu["typeLocked"] = isTypeLocked();
	serializeJson(docu, result);
	return result;
}