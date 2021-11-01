#include <Arduino.h>
#include <Model/HTTP_resp.h>

#define PIN_MAX                 10

#define mode_INPUT             "INPUT"
#define mode_INPUT_PULLDOWN    "INPUT_PULLDOWN"
#define mode_INPUT_PULLUP      "INPUT_PULLUP"
#define mode_OUTPUT            "OUTPUT"
#define mode_OUTPUT_OPEN_DRAIN "OUTPUT_OPEN_DRAIN"
#define mode_ANALOG            "ANALOG"
#define state_HIGH             "HIGH"
#define state_LOW              "LOW"



struct Pin {
    byte pinNumber;
    byte mode;
    byte standby;
    unsigned long defaultLatchTimeMs;
    unsigned long latchTimeMs;
};

String setPinMap(int pinNumber, String pinMod, String pinStandby, int pinLatchTime);
String writePin(int pin, String state);
void pinsReset();
void scanPins();
