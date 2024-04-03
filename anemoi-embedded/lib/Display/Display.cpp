#include "Display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Display::Display(logging::Logger *logger) {
  _isAvailable = false;

  _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
  _logger = logger;
}

void Display::setup() {
  if (!_display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    _logger->log(
            logging::LoggerLevel::LOGGER_LEVEL_ERROR,
            "DISPLAY",
            "Error initializing the display");
    return;
  }

  _isAvailable = true;
}

bool Display::isAvailable() {
  return _isAvailable;
}

void Display::showDisplayCalibratingMessage() {
  if (!isAvailable()) return;

  _display->clearDisplay();

  _display->setTextSize(1);
  _display->setTextColor(WHITE);
  _display->setCursor(0, 10);

  _display->println("Calibrating...");
  _display->display();
}

void Display::showDisplayMessage(String message) {
  if (!isAvailable()) return;

  _display->clearDisplay();

  _display->setTextSize(2);
  _display->setTextColor(WHITE);
  _display->setCursor(0, 10);

  _display->println(message);
  _display->display();
}

void Display::showDisplayGasInformation(float o2SensorVoltage, float percentageO2) {
  if (!isAvailable()) return;

  _display->clearDisplay();

  _display->setTextSize(2);
  _display->setCursor(0, 5);
  _display->print("O2");

  _display->setCursor(42, 5);
  _display->print(o2SensorVoltage);
  _display->print("mV");

  _display->setTextSize(4);
  _display->setCursor(0, 30);
  _display->print(String(percentageO2, 1));
  _display->println("%");
  _display->display();
}