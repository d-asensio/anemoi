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
#define CHARACTERISTIC_UUID_RX "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"
#define CHARACTERISTIC_UUID_TX "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"

#define ATMOSPHERIC_O2_FRACTION 0.21

const uint8_t qrCodeVersion = 3;
const uint8_t pixelSize = 2;

RTC_DS3231 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;

float atmosphericCellVoltage = 0;
float atmosphericPressureBars = 0;

BLEServer *pServer = nullptr;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

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
    }
  }
};

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

float readAtmosphericPressureBars() {
  float atmospheric_pressure_pascals = bmp.readPressure();

  return atmospheric_pressure_pascals / 100000;
}

float getO2PartialPressureFromVoltage(float currentCellVoltage) {
  return ATMOSPHERIC_O2_FRACTION * currentCellVoltage / atmosphericCellVoltage;
}

void calibrateO2Sensor() {
  atmosphericCellVoltage = readO2SensorVoltage();
  atmosphericPressureBars = readAtmosphericPressureBars();
}

void showCalibratingMessage() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  display.println("Calibrating...");
  display.display();
}

void initBLEConnection() {
  BLEDevice::init("Anemoi Analyzer Nano");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new AnemoiBLEServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
          CHARACTERISTIC_UUID_TX,
          BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
          CHARACTERISTIC_UUID_RX,
          BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new AnemoiBLECharacteristicCallbacks());

  // Start the service
  pService->start();

  // Start advertising
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
}

void loop() {
  float o2SensorVoltage = readO2SensorVoltage();
  float ppO2 = getO2PartialPressureFromVoltage(o2SensorVoltage);

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

    char ppO2Temp[50];
    dtostrf(ppO2, 6, 2, ppO2Temp);
    pTxCharacteristic->setValue(ppO2Temp);
    pTxCharacteristic->notify();
    // Bluetooth stack will go into congestion, if too many packets are sent
    delay(10);
  }

  // Disconnecting
  if (!deviceConnected && oldDeviceConnected) {
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
  }

  display.setTextSize(2);

  display.clearDisplay();
  display.setCursor(0, 5);
  display.print(o2SensorVoltage);
  display.println(" mV");

  display.setCursor(0, 25);
  display.print(ppO2);
  display.println(" ppO2");

  display.setCursor(0, 45);
  display.print(atmosphericPressureBars);
  display.println(" bar");
  display.display();

  if (!bmp.takeForcedMeasurement()) {
    Serial.println("BMP reading measurement failed!");
  }
}