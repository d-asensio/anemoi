#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_SSD1306.h>

void initDisplayModule();
void showDisplayCalibratingMessage();
void showDisplayMessage(String message);
void showDisplayGasInformation (float o2SensorVoltage, float percentageO2);