#include <WiFi/WiFi_Network.h>

void waitTillConnected() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		digitalWrite(BUILTIN_LED,LOW);
		delay(50);
		digitalWrite(BUILTIN_LED,HIGH);
		delay(50);
  }
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
