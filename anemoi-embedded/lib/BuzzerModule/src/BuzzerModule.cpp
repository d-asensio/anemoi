#include "BuzzerModule.h"

#define BUZZER_PIN 32

void initBuzzerModule() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void buzzerBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}
