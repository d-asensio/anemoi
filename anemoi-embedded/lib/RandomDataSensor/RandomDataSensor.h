#include <Arduino.h>

#include "ISensor.h"

#ifndef RANDOM_DATA_SENSOR_H
#define RANDOM_DATA_SENSOR_H

class RandomDataSensor : public ISensor {
  public:
  RandomDataSensor(float lowerBound, float upperBound);

  virtual void setup();
  virtual void loop();
  virtual bool isAvailable();
  virtual float read();

  private:
  float _randomFloat(float lowerBound, float upperBound);

  float _lowerBound;
  float _upperBound;
};

#endif