#include "Buzzer.h"

#define BUZZER_PIN 32

Buzzer::Buzzer(logging::Logger *logger) {
  _logger = logger;
}

void Buzzer::setup() {

    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_INFO,
            "Buzzer",
            "Buzzer initialized");
}

void Buzzer::beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}
