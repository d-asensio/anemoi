#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(logging::Logger *logger, Adafruit_BMP280 *bmp) {
  _isAvailable = false;
  _lastReading = 0;

  _bmp = bmp;
  _logger = logger;
}

void TemperatureSensor::setup() {
  if (!_bmp->begin(0x76)) {
    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_ERROR,
            "TEMPERATURE_SENSOR",
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

void TemperatureSensor::loop() {
  if (!isAvailable()) return;

  if (!_bmp->takeForcedMeasurement()) {
    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_WARN,
            "TEMPERATURE_SENSOR",
            "Error reading the sensor");

    return;
  }

  _lastReading = _bmp->readTemperature();
}

bool TemperatureSensor::isAvailable() {
  return _isAvailable;
}


float TemperatureSensor::read() {
  return _lastReading;
}