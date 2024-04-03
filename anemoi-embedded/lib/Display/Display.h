#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "logger.h"

#ifndef DISPLAY_H
#define DISPLAY_H

class Display {
  public:
  Display(logging::Logger *logger);

  void setup();
  bool isAvailable();

  void showDisplayCalibratingMessage();
  void showDisplayMessage(String message);
  void showDisplayGasInformation(float o2SensorVoltage, float percentageO2);

  private:
  bool _isAvailable;

  Adafruit_SSD1306 *_display;
  logging::Logger *_logger;
};

#endif