#include "RandomDataSensor.h"

RandomDataSensor::RandomDataSensor(float lowerBound, float upperBound) {
  _lowerBound = lowerBound;
  _upperBound = upperBound;
}

void RandomDataSensor::setup() {}

void RandomDataSensor::loop() {}

bool RandomDataSensor::isAvailable() {
  return true;
}

float RandomDataSensor::read() {
  return _randomFloat(_lowerBound, _upperBound);
}

float RandomDataSensor::_randomFloat(float lowerBound, float upperBound) {
  return random(0, (upperBound - lowerBound) * 10000) / 10000.0 + lowerBound;
}