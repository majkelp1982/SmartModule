#include <WiFi/WiFi_Network.h>

bool isConnected = false;

void waitTillConnected() {
	while (WiFi.status() != WL_CONNECTED) {
		isConnected = false;
		Serial.print(".");
		digitalWrite(BUILTIN_LED,LOW);
		delay(50);
		digitalWrite(BUILTIN_LED,HIGH);
		delay(50);
  }
  if (!isConnected) {
	  sendStatusToModulManager();
	  Serial.println(WiFi.localIP());
  }

isConnected = true;
}
  

void WiFi_network() {  
	pinMode(BUILTIN_LED, OUTPUT);
	WiFi.begin("Majkel", "12345678");
    Serial.print("WIFI status = ");
    Serial.println(WiFi.getMode());
	waitTillConnected();
}


void WiFi_conectionCheck() {
		if (!WiFi.isConnected()) {
			WiFi.reconnect();
		}
		waitTillConnected();
}

String getMacAddress() {
	return WiFi.macAddress();
}

void sendStatusToModulManager() {
	Serial.print("\nSend params to module manager");
	WiFiClient client;
	HTTPClient http;
	http.begin(client, MANAGERURL);
	http.addHeader("Content-Type", "application/json");
	int httpCode = http.POST(getDeviceParams());

	Serial.printf("\nHTTP Code:%i",httpCode);
	http.end();
}
