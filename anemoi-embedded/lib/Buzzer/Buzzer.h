#include <Arduino.h>

#include "logger.h"

#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {
  public:
  Buzzer(logging::Logger *logger);

  void setup();

  void beep();

  private:
  bool _isAvailable;

  logging::Logger *_logger;
};

#endif