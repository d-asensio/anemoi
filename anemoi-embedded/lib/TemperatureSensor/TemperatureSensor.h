#include "ISensor.h"
#include "logger.h"

#include <Adafruit_BMP280.h>

#ifndef TEMPERATURE_VOLTAGE_SENSOR_H
#define TEMPERATURE_VOLTAGE_SENSOR_H

class TemperatureSensor : public ISensor {
public:
    TemperatureSensor(logging::Logger* logger, Adafruit_BMP280* bmp);

    virtual void setup();
    virtual void loop();
    virtual bool isAvailable();
    virtual float read();
private:
    bool _isAvailable;
    float _lastReading;

    Adafruit_BMP280* _bmp;
    logging::Logger* _logger;
};

#endif