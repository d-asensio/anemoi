#include "DisplayModule.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initDisplayModule() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) continue;
  }
}
void showDisplayCalibratingMessage() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  display.println("Calibrating...");
  display.display();
}

void showDisplayMessage(String message) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  display.println(message);
  display.display();
}

void showDisplayGasInformation (float o2SensorVoltage, float percentageO2) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 5);
  display.print("O2");

  display.setCursor(42, 5);
  display.print(o2SensorVoltage);
  display.print("mV");

  display.setTextSize(4);
  display.setCursor(0, 30);
  display.print(String(percentageO2, 1));
  display.println("%");
  display.display();
}