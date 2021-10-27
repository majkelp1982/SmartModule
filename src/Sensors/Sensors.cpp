#include <Sensors/Sensors.h>
#include <Model/HTTP_resp.h>

String sensorBME280SPIread(int sensorIndex);

Sensor sensors[SENSORS_MAX];
int nextFreeSensor = 0;

String sensorRead(String name) {
    Serial.print("\nsensorRead");
    int sensorIndex = -1;
    Serial.print("\nSensor position search. Name:"+name);
    for (int i=0; i<nextFreeSensor; i++) {
        Serial.printf("\nList[%d] sensor type:%s, name:%s ",i,sensors[i].type,sensors[i].name);
        if (sensors[i].name == name)
            sensorIndex = i;
    }
    if (sensorIndex == -1) {
        String result = RESP_SENSOR_NOT_FOUND;
        result.concat(" name:");
        result.concat(name);
        return result;
    }
    String response = "";

    Serial.printf("\nSensor found on position %d",sensorIndex); 
    if (sensors[sensorIndex].type == BME280SPI)
        response = sensorBME280SPIread(sensorIndex);
    else return RESP_SENSOR_TYPE_NOT_RECOGNIZED;

    return response;
}

String sensorBME280SPI(String name, int8_t csPin) {
    if (csPin == 0)
        return RESP_CS_PIN_MISSING;
    Adafruit_BME280* sensor = new Adafruit_BME280(csPin, SPI_MOSI, SPI_MISO, SPI_SCK);
    sensors[nextFreeSensor].name = name;
    sensors[nextFreeSensor].type = BME280SPI;
    sensors[nextFreeSensor].bme280 = sensor;
    sensors[nextFreeSensor].bme280->begin();
    Serial.printf("\nSensor BME280SPI saved on position %d, name:%s, csPin:%d",nextFreeSensor, sensors[nextFreeSensor].name, csPin);
    nextFreeSensor++;  
    return RESP_OK;
}

String sensorBME280SPIread(int sensorIndex) {
    Serial.print("\nsensorBME280SPIread");
    float temperature = sensors[sensorIndex].bme280->readTemperature();
    float pressure = sensors[sensorIndex].bme280->readPressure();
    float humidity = sensors[sensorIndex].bme280->readHumidity();
    String response="{\""+sensors[sensorIndex].name+"\":{\"temperature\":";
    response.concat(temperature);
    response.concat(",\"pressure:");
    response.concat(pressure);
    response.concat(",\"humidity:");
    response.concat(humidity);
    response.concat("},");
    Serial.print("\nsensorBEM280SPIread done");
    return response;
}

void sensorSDS011(String name, int csPin) {
    SDS011* sensor = new SDS011(SERIAL2_RX, SERIAL2_TX);
    sensors[nextFreeSensor].name = name;
    sensors[nextFreeSensor].type = SDS011SERIAL;
    sensors[nextFreeSensor].sds011 = sensor;  
    sensors[nextFreeSensor].sds011->begin();
    nextFreeSensor++;  
}

void sensorsReset() {
    for (int i=0; i<nextFreeSensor; i++) {
        sensors[i].name = "";
        sensors[i].type = "";
    }
    nextFreeSensor = 0;
}
