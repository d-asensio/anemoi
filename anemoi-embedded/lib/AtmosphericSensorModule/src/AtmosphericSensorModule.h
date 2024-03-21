#include <Arduino.h>

#include <Adafruit_BMP280.h>

void initAtmosphericSensorModule();
float readAtmosphericPressure();
float readTemperature();
void atmosphericSensorModuleLoop();
