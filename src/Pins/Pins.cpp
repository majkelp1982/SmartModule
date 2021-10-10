#include <Pins/Pins.h>

Pin pins[PIN_MAX];
byte mapper[40];
int nextFree = 0;

String setPinMap(int pinInt, String modeStr, String standbyStr) {
    Serial.print("\nsetPinMap\n pin:"); Serial.print(pinInt); Serial.print(", Mode:"+modeStr); Serial.print(", standby:"+standbyStr);
    Serial.print(" register on pos free:");
    Serial.println(nextFree);   
    int modeInt=0;
    if (modeStr == mode_INPUT) modeInt = INPUT;
    else if (modeStr == mode_INPUT_PULLDOWN) modeInt = INPUT_PULLDOWN;
    else if (modeStr == mode_INPUT_PULLUP) modeInt = INPUT_PULLUP;
    else if (modeStr == mode_OUTPUT) modeInt = OUTPUT;
    else if (modeStr == mode_OUTPUT_OPEN_DRAIN) modeInt = OUTPUT_OPEN_DRAIN;
    else if (modeStr == mode_ANALOG) modeInt = ANALOG;
    else {
          String result = RESP_PIN_BAD_DECLARATION;
          result.concat(" on pin:");
          result.concat(pinInt);
          result.concat(" mode "+modeStr+" is not recognized.");
          return result;
    }

    int standbyInt = 0;
    if (standbyStr == state_HIGH) standbyInt = HIGH;
    else if (standbyStr == state_LOW) standbyInt = LOW; 
    else {
          String result = RESP_PIN_BAD_DECLARATION;
          result.concat(" on pin:");
          result.concat(pinInt);
      result = RESP_PIN_BAD_DECLARATION;
      result.concat(" standby "+standbyStr+" is not recognized.");
      return result;
    }
    mapper[pinInt] = nextFree;
    pins[nextFree].mode = modeInt;
    pins[nextFree].standby = standbyInt;
    pins[nextFree].pinNumber = pinInt;
    nextFree++;

    pinMode(pinInt, pins[mapper[pinInt]].mode);
    digitalWrite(pinInt, pins[mapper[pinInt]].standby);
    Serial.print("\nsetPinMap done");
    return RESP_OK;
}

String writePin(String pin, String state, String latchTimeMs) {
    Serial.print("\nwritePin pin:"+pin+ " state:"+state+" latchTime:"+latchTimeMs);

    int pinInt = pin.toInt();
    int latchTimeMsInt = latchTimeMs.toInt();
    if ((pinInt == 0) || (latchTimeMsInt == 0))
        return RESP_PIN_BAD_DECLARATION;
    byte pinState = (state == state_LOW)? LOW : (state == state_HIGH)? HIGH : 10;
    if (pinState == 10) {
        String resp = RESP_PIN_BAD_DECLARATION;
        resp.concat(" pin:"+pin+" state:"+state+" not recognized");
        return RESP_PIN_BAD_DECLARATION;
    }

    pins[mapper[pinInt]].latchTimeMs = millis() + latchTimeMsInt;

    digitalWrite(pinInt, pinState);
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

