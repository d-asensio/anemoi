#include "GasSensorsModule.h"

Adafruit_ADS1115 ads;

void initGasSensorsModule() {
  if (!ads.begin()) {
    Serial.println(F("Failed to initialize ADS."));
    while (true) continue;
  }
}

float readO2SensorVoltage() {
  float multiplier = 0.1875F;
  float adc_reading = ads.readADC_Differential_0_1();

  return adc_reading * multiplier;
}
