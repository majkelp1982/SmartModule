#include <WebService/WebService.h>
#include <WebService/OTA.h>
#include <RESTAPI/configuration.h>
#include <Model/HTTP_resp.h>
#include <RESTAPI/service.h>
#include <WiFi/WiFi_Network.h>

WebServer server(80);

void webServerStart();
void webServerSetup();

void webService_setup() {
    OTA_setup();
    webServerStart();
    webServerSetup();
}

void webServerStart() {
  	  /*use mdns for host name resolution*/
	  if (!MDNS.begin("esp32")) { //http://esp32.local
	    Serial.println("Error setting up MDNS responder!");
	    while (1) {
	      delay(1000);
	    }
	  }
	  Serial.println("mDNS responder started");
}

//Endpoints
// /   					GET		- login page
// /serverIndex 		GET		- upload page
// /update				GET		- handlin firmware OTA file
// /configuration		POST	- save configuration
// /action				POST	- handling pins/sensors action
// /params				GET		- retvieve macAddress, connection interruptions, etc.
// /unlockType			POST	- To change modul type, type need to be unlocked

void webServerSetup() {
	//Login page
	  server.on("/", HTTP_GET, []() {
	    server.sendHeader("Connection", "close");
	    server.send(200, "text/html", getLoginPage());
	  });

	  //Upload page
	  server.on("/serverIndex", HTTP_GET, []() {
	    server.sendHeader("Connection", "close");
	    server.send(200, "text/html", getUploadPage());
	  });

	  /*handling uploading firmware file OTA*/
	  server.on("/update", HTTP_POST, []() {
	    server.sendHeader("Connection", "close");
	    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
	    ESP.restart();
	  }, []() {
	    HTTPUpload& upload = server.upload();
	    if (upload.status == UPLOAD_FILE_START) {
	    	Serial.printf("Update progress: %s\n", upload.filename.c_str());
	    	if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
	    		Update.printError(Serial);
	      }
	    } else if (upload.status == UPLOAD_FILE_WRITE) {
	      /* flashing firmware to ESP*/
	      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
	        Update.printError(Serial);
	      }
	    } else if (upload.status == UPLOAD_FILE_END) {
	      if (Update.end(true)) { //true to set the size to the current progress
	        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
	      } else {
	        Update.printError(Serial);
	      }
	    }
	  });

	  //Module
	  //Configuration
	  server.on("/configuration", HTTP_POST, []() {
		if (server.hasArg("plain") == false) {
			server.send(404, "text/plain", RESP_JSON_NOT_FOUND);
			return;
		}
		Serial.println("Configuation start");
		String response = configuration(server.arg("plain"));
		Serial.println("Configuration end. Response: "+response);
		if (response == RESP_OK) 
		    setLoginPage();
	    server.send((response==RESP_OK)?200:400, "text/plain", response); 
	  });

	  //Service
	   server.on("/action", HTTP_POST, []() {
		if (server.hasArg("plain") == false) {
			server.send(404, "text/plain", RESP_JSON_NOT_FOUND);
			return;
		}
		Serial.print("\nAction start");
		String response = action(server.arg("plain"));
		Serial.print("\nAction end. Response: "+response);
	    server.send((response==RESP_OK)?200:400, "text/plain", response); 
	  });

	  //Params
	   server.on("/params", HTTP_GET, []() {
	    server.sendHeader("Connection", "close");
		server.send(200, "text/html", getDeviceParams());
	  });	

	server.on("/unlockType", HTTP_POST, []() {
		Serial.print("\nunlockType start");
		setTypeLocked(false);
		Serial.print("\nunlockType done.");
	    server.send(200, "text/plain", RESP_OK); 
	  });
	  server.begin();
}

void webService_run() {
    server.handleClient();
}