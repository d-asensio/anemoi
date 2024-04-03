#include "O2VoltageSensor.h"

O2VoltageSensor::O2VoltageSensor(logging::Logger *logger, Adafruit_ADS1115 *ads) {
  _isAvailable = false;
  _lastReading = 0;

  _ads = ads;
  _logger = logger;
}

void O2VoltageSensor::setup() {
  if (!_ads->begin()) {
    _logger->log(
      logging::LoggerLevel::LOGGER_LEVEL_ERROR,
      "O2_VOLTAGE_SENSOR",
      "Error initializing the sensor"
    );
    return;
  }

  _isAvailable = true;
}

void O2VoltageSensor::loop() {
  if(!isAvailable()) return;

  _lastReading = _ads->readADC_Differential_0_1();
}

bool O2VoltageSensor::isAvailable() {
  return _isAvailable;
}


float O2VoltageSensor::read() {
  return _lastReading * 0.1875F;
}