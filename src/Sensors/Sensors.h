#include <Arduino.h>
#include <Model/const_pin.h>
#include <Adafruit_BME280.h>
#include <Sensors/Sds011.h>

#define SENSORS_MAX         20

#define BME280SPI           "BME280SPI"
#define SDS011SERIAL        "SDS011SERIAL"

struct Sensor {
    String name;
    String type;
    Adafruit_BME280* bme280;    
    SDS011* sds011;
};

void sensorsReset();
String sensorRead(String name);
String sensorBME280SPI(String name, int8_t csPin);
