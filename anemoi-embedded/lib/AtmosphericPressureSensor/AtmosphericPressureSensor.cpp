#include "AtmosphericPressureSensor.h"

AtmosphericPressureSensor::AtmosphericPressureSensor(logging::Logger *logger, Adafruit_BMP280 *bmp) {
  _isAvailable = false;
  _lastReading = 0;

  _bmp = bmp;
  _logger = logger;
}

void AtmosphericPressureSensor::setup() {
  if (!_bmp->begin(0x76)) {
    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_ERROR,
            "ATMOSPHERIC_PRESSURE_SENSOR",
            "Error initializing the sensor");
    return;
  }

  _bmp->setSampling(Adafruit_BMP280::MODE_FORCED,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);

  _isAvailable = true;
}

void AtmosphericPressureSensor::loop() {
  if (!isAvailable()) return;

  if (!_bmp->takeForcedMeasurement()) {
    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_WARN,
            "ATMOSPHERIC_PRESSURE_SENSOR",
            "Error reading the sensor");

    return;
  }

  _lastReading = _bmp->readPressure() / 100;// in millibars
}

bool AtmosphericPressureSensor::isAvailable() {
  return _isAvailable;
}


float AtmosphericPressureSensor::read() {
  return _lastReading;
}