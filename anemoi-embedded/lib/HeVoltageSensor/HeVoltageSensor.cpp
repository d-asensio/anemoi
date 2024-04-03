#include "HeVoltageSensor.h"

HeVoltageSensor::HeVoltageSensor(logging::Logger* logger, Adafruit_ADS1115* ads) {
  _isAvailable = false;
  _lastReading = 0;

  _ads = ads;
  _logger = logger;
}

void HeVoltageSensor::setup() {
  if (!_ads->begin()) {
    _logger->log(
      logging::LoggerLevel::LOGGER_LEVEL_ERROR,
      "HE_VOLTAGE_SENSOR",
      "Error initializing the sensor"
    );
    return;
  }

  _isAvailable = true;
}

void HeVoltageSensor::loop() {
  if(!isAvailable()) return;

  _lastReading = _ads->readADC_Differential_2_3();
}

bool HeVoltageSensor::isAvailable() {
  return _isAvailable;
}


float HeVoltageSensor::read() {
  return _lastReading * 0.1875F;
}