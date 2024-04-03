#include "ISensor.h"
#include "logger.h"

#include <Adafruit_BMP280.h>

#ifndef ATMOSPHERIC_PRESSURE_SENSOR_H
#define ATMOSPHERIC_PRESSURE_SENSOR_H

class AtmosphericPressureSensor : public ISensor {
  public:
  AtmosphericPressureSensor(logging::Logger *logger, Adafruit_BMP280 *bmp);

  virtual void setup();
  virtual void loop();
  virtual bool isAvailable();
  virtual float read();

  private:
  bool _isAvailable;
  float _lastReading;

  Adafruit_BMP280 *_bmp;
  logging::Logger *_logger;
};

#endif