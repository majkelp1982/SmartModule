#include <Pins/Pins.h>

Pin pins[PIN_MAX];
byte mapper[40];
int nextFree = 0;

String setPinMap(int pinNumber, String pinMod, String pinStandby, int pinLatchTime) {
    Serial.print("\nsetPinMap\n pin:"); Serial.print(pinNumber); Serial.print(", Mode:"+pinMod); Serial.print(", standby:"+pinStandby);  Serial.print(", latchTime:"); Serial.print(pinLatchTime);
    Serial.print(" register on pos free:");
    Serial.println(nextFree);   
    int modeInt=0;
    if (pinMod == mode_INPUT) modeInt = INPUT;
    else if (pinMod == mode_INPUT_PULLDOWN) modeInt = INPUT_PULLDOWN;
    else if (pinMod == mode_INPUT_PULLUP) modeInt = INPUT_PULLUP;
    else if (pinMod == mode_OUTPUT) modeInt = OUTPUT;
    else if (pinMod == mode_OUTPUT_OPEN_DRAIN) modeInt = OUTPUT_OPEN_DRAIN;
    else if (pinMod == mode_ANALOG) modeInt = ANALOG;
    else {
          String result = RESP_PIN_BAD_DECLARATION;
          result.concat(" on pin:");
          result.concat(pinNumber);
          result.concat(" mode "+pinMod+" is not recognized.");
          return result;
    }

    int standbyInt = -1;
    if (pinStandby == state_HIGH) standbyInt = HIGH;
    else if (pinStandby == state_LOW) standbyInt = LOW;

    if ((modeInt == OUTPUT || modeInt == OUTPUT_OPEN_DRAIN) && standbyInt == -1) {
          String result = RESP_PIN_BAD_DECLARATION;
          result.concat(". Pin:");
          result.concat(pinNumber);
          result.concat(" is an output. Standby "+pinStandby+" is not recognized.");
      return result;
    }
    
    mapper[pinNumber] = nextFree;
    pins[nextFree].mode = modeInt;
    pins[nextFree].standby = standbyInt;
    pins[nextFree].pinNumber = pinNumber;
    pins[nextFree].latchTimeMs = pinLatchTime;
    nextFree++;

    pinMode(pinNumber, pins[mapper[pinNumber]].mode);
    if (modeInt == OUTPUT && modeInt == OUTPUT_OPEN_DRAIN) 
        digitalWrite(pinNumber, pins[mapper[pinNumber]].standby);
    Serial.print("\nsetPinMap done");
    return RESP_OK;
}

String writePin(int pin, String state) {
    Serial.printf("\nwritePin pin:%d state: %s",pin, state);
    if (pin == 0)
        return RESP_PIN_BAD_DECLARATION;
    byte pinState = (state == state_LOW)? LOW : (state == state_HIGH)? HIGH : 10;
    if (pinState == 10) {
        String resp = RESP_PIN_BAD_DECLARATION;
        resp.concat(" pin:");
        resp.concat(pin);
        resp.concat(" state:"+state+" not recognized");
        return RESP_PIN_BAD_DECLARATION;
    }

    pins[mapper[pin]].latchTimeMs = millis() + pins[mapper[pin]].defaultLatchTimeMs;

    digitalWrite(pin, pinState);
    return RESP_OK;
}

void scanPins() {
    unsigned long currentMillis = millis();
    for (int i=0; i<nextFree; i++) {
        if (currentMillis>pins[i].latchTimeMs)
            digitalWrite(pins[i].pinNumber, pins[i].standby);
    }
}

void pinsReset() {
    Serial.print("\nPins reset");
    for (int i=0; i<nextFree; i++) {
        pins[i].mode=0;
        pins[i].pinNumber = 0;
        pins[i].standby = 0;
        pins[i].latchTimeMs = 0;
    }
    nextFree = 0;  
    Serial.print("\nPins reset done");
}

