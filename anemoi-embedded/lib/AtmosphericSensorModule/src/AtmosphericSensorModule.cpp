#include "AtmosphericSensorModule.h"

Adafruit_BMP280 bmp;

void initAtmosphericSensorModule() {
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    while (true) continue;
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

float readAtmosphericPressure() {
  return bmp.readPressure() / 100; // in millibars
}

float readTemperature() {
  return bmp.readTemperature();
}

void atmosphericSensorModuleLoop() {
  if (!bmp.takeForcedMeasurement()) {
    Serial.println("BMP reading measurement failed!");
  }
}