#include <Arduino.h>
#include <Wire.h>

#include <RTClib.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include <Adafruit_ADS1X15.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SSD1306.h>

#define BUZZER_PIN 32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SERVICE_UUID "9243e98a-314c-42b2-a4fc-c23d54f0f271"

#define O2_PERCENTAGE_CHARACTERISTIC_UUID "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"
#define ATMOSPHERIC_PRESSURE_CHARACTERISTIC_UUID "68848368-6d91-49f9-9a5f-fed73463c9f6"
#define TEMPERATURE_PRESSURE_CHARACTERISTIC_UUID "a9bac333-e37c-42a9-8abc-9b07350e189d"
#define CALIBRATE_SIGNAL_CHARACTERISTIC_UUID "8d07c070-b5e0-4859-bc71-88b425e040c0"

#define ATMOSPHERIC_O2_FRACTION_AT_SEA_LEVEL 0.209
#define ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL 1013.25 // in millibars

const uint8_t qrCodeVersion = 3;
const uint8_t pixelSize = 2;

RTC_DS3231 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;

float calibrationO2SensorVoltage = 0;
float calibrationAtmosphericPressure = 0;

BLEServer *pServer = nullptr;
BLECharacteristic *percentageO2Characteristic;
BLECharacteristic *atmosphericPresureO2Characteristic;
BLECharacteristic *temperatureCharacteristic;
BLECharacteristic *callibrateSignalCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

bool callibrateSignalReceived = false;

void initI2CRTC() {
  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (true) continue;
  }

  // Uncomment to set date (comment again)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void initBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void initI2CBpm() {
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

void initI2CAds() {
  if (!ads.begin()) {
    Serial.println(F("Failed to initialize ADS."));
    while (true) continue;
  }
}

void initI2CDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) continue;
  }
}

float readO2SensorVoltage() {
  float multiplier = 0.1875F;
  float adc_reading = ads.readADC_Differential_0_1();

  return adc_reading * multiplier;
}

float readAtmosphericPressure() {
  return bmp.readPressure() / 100;// in millibars
}

float readTemperature() {
  return bmp.readTemperature();
}

float getO2PartialPressureFromVoltage(float currentCellVoltage) {
  return ATMOSPHERIC_O2_FRACTION_AT_SEA_LEVEL * currentCellVoltage / calibrationO2SensorVoltage;
}

void calibrateO2Sensor() {
  calibrationO2SensorVoltage = readO2SensorVoltage();
  calibrationAtmosphericPressure = readAtmosphericPressure();
}

void showCalibratingMessage() {
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

void buzzerBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

class AnemoiBLEServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *bleServer) override {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *bleServer) override {
    deviceConnected = false;
  }
};

class AnemoiBLECharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string rxValue = pCharacteristic->getValue();

    if (!rxValue.empty()) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");

      callibrateSignalReceived = true;
    }
  }
};

void initBLEConnection() {
  BLEDevice::init("Anemoi Analyzer Nano");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new AnemoiBLEServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  percentageO2Characteristic = pService->createCharacteristic(
          O2_PERCENTAGE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  percentageO2Characteristic->addDescriptor(new BLE2902());

  atmosphericPresureO2Characteristic = pService->createCharacteristic(
          ATMOSPHERIC_PRESSURE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  atmosphericPresureO2Characteristic->addDescriptor(new BLE2902());

  temperatureCharacteristic = pService->createCharacteristic(
          TEMPERATURE_PRESSURE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  temperatureCharacteristic->addDescriptor(new BLE2902());

  callibrateSignalCharacteristic = pService->createCharacteristic(
          CALIBRATE_SIGNAL_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_WRITE);

  callibrateSignalCharacteristic->setCallbacks(new AnemoiBLECharacteristicCallbacks());

  pService->start();

  pServer->getAdvertising()->start();

  Serial.println("Start BLE advertising");
}

void setup() {
  Serial.begin(115200);
  initBLEConnection();

  initBuzzer();

  initI2CRTC();
  initI2CDisplay();
  initI2CAds();
  initI2CBpm();

  calibrateO2Sensor();
  showCalibratingMessage();
  buzzerBeep();
}

void loop() {
  float o2SensorVoltage = readO2SensorVoltage();
  float ppO2 = getO2PartialPressureFromVoltage(o2SensorVoltage);
  float currentAtmosphericPressure = readAtmosphericPressure();

  float percentageO2 = ppO2 * 100;

  float temperature = readTemperature();

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

  if (callibrateSignalReceived) {
    buzzerBeep();
    calibrateO2Sensor();
    showCalibratingMessage();
    delay(1000);

    callibrateSignalReceived = false;
  }

  if (deviceConnected) {
    DateTime now = rtc.now();

    Serial.print("RTC Date Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print('-');
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    // Send atmospheticPressure data
    char atmosphericPressureTmp[50];
    dtostrf(currentAtmosphericPressure, 6, 2, atmosphericPressureTmp);
    atmosphericPresureO2Characteristic->setValue(atmosphericPressureTmp);
    atmosphericPresureO2Characteristic->notify();

    // Send temperature data
    char temperatureTmp[50];
    dtostrf(temperature, 6, 2, temperatureTmp);
    temperatureCharacteristic->setValue(temperatureTmp);
    temperatureCharacteristic->notify();

    // Send O2 percentage data
    char percentageO2Tmp[50];
    dtostrf(percentageO2, 6, 2, percentageO2Tmp);
    percentageO2Characteristic->setValue(percentageO2Tmp);
    percentageO2Characteristic->notify();

    delay(10);// Avoid BLE data congestion
  }

  // Disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    buzzerBeep();
    showDisplayMessage("Disconnected!");
    // Give the bluetooth stack the chance to get things ready
    delay(500);

    // Restart advertising
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;

    Serial.println("-- Client disconnected! --");
  }

  // Connecting
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("-- Client connected --");
    oldDeviceConnected = deviceConnected;

    buzzerBeep();
    showDisplayMessage("Connected!");
    delay(500);
  }

  if (!bmp.takeForcedMeasurement()) {
    Serial.println("BMP reading measurement failed!");
  }
}
