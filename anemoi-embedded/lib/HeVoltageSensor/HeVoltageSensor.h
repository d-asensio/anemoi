#include "ISensor.h"
#include "logger.h"

#include <Adafruit_ADS1X15.h>

#ifndef HE_CELL_VOLTAGE_SENSOR_H
#define HE_CELL_VOLTAGE_SENSOR_H

class HeVoltageSensor : public ISensor<float> {
public:
    HeVoltageSensor(logging::Logger* logger, Adafruit_ADS1115* ads);

    virtual void setup();
    virtual void loop();
    virtual bool isAvailable();
    virtual float read();

private:
    bool _isAvailable;
    float _lastReading;

    Adafruit_ADS1115* _ads;
    logging::Logger* _logger;
};

#endif