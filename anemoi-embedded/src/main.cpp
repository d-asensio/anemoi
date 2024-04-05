#include <Arduino.h>
#include <Wire.h>

#include <ArduinoJson.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include <RTClib.h>

#include "ISensor.h"

#include "Buzzer.h"
#include "RTCTime.h"
#include "Display.h"

#define SERVICE_UUID "9243e98a-314c-42b2-a4fc-c23d54f0f271"

#define TX_CHARACTERISTIC_UUID "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"
#define CALIBRATE_SIGNAL_CHARACTERISTIC_UUID "8d07c070-b5e0-4859-bc71-88b425e040c0"

#define ATMOSPHERIC_O2_PERCENTAGE_AT_SEA_LEVEL 0.209
#define ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL 1013.25// in millibars

///
logging::Logger logger;

#if DUMMY_BOARD_MODE == 1
#include "RandomDataSensor.h"

ISensor<float> *heVoltageSensor = new RandomDataSensor(0, 2);
ISensor<float> *o2VoltageSensor = new RandomDataSensor(9, 11);
ISensor<float> *atmosphericPressureSensor = new RandomDataSensor(1013, 1024);
ISensor<float> *temperatureSensor = new RandomDataSensor(19, 22);
#else
#include <Adafruit_ADS1X15.h>
#include <Adafruit_BMP280.h>

#include "AtmosphericPressureSensor.h"
#include "HeVoltageSensor.h"
#include "O2VoltageSensor.h"
#include "TemperatureSensor.h"

Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;

ISensor<float> *heVoltageSensor = new HeVoltageSensor(&logger, &ads);
ISensor<float> *o2VoltageSensor = new O2VoltageSensor(&logger, &ads);
ISensor<float> *atmosphericPressureSensor = new AtmosphericPressureSensor(&logger, &bmp);
ISensor<float> *temperatureSensor = new TemperatureSensor(&logger, &bmp);
#endif

RTC_DS3231 rtc;

Display display(&logger);
Buzzer buzzer(&logger);
RTCTime rtcTime(&logger, &rtc);
///

float calibrationO2SensorVoltage = 0;
float calibrationHeSensorVoltage = 0;
float calibrationAtmosphericPressure = 0;
bool callibrateSignalReceived = false;

BLEServer *pServer = nullptr;
BLECharacteristic *txCharacteristic;
BLECharacteristic *callibrateSignalCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

float getAtmosphericO2Percentage(float currentAtmosphericPressure) {
  return (ATMOSPHERIC_O2_PERCENTAGE_AT_SEA_LEVEL * currentAtmosphericPressure) / ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL;
}

float getO2FractionFromVoltage(float currentCellVoltage, float atmosphericO2Percentage) {
  return atmosphericO2Percentage * currentCellVoltage / calibrationO2SensorVoltage;
}

float getHeFractionFromVoltage(float currentHeSensorVoltage) {
  return .75 / 429.00 * (currentHeSensorVoltage - calibrationHeSensorVoltage);
}

void calibrateSensors() {
  calibrationO2SensorVoltage = o2VoltageSensor->read();
  calibrationAtmosphericPressure = atmosphericPressureSensor->read();
  calibrationHeSensorVoltage = heVoltageSensor->read();
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

  txCharacteristic = pService->createCharacteristic(
          TX_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  txCharacteristic->addDescriptor(new BLE2902());

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
  logger.setSerial(&Serial);

  o2VoltageSensor->setup();
  heVoltageSensor->setup();
  atmosphericPressureSensor->setup();
  temperatureSensor->setup();

  display.setup();
  buzzer.setup();
  rtcTime.setup();

  initBLEConnection();

  calibrateSensors();
  display.showDisplayCalibratingMessage();
  buzzer.beep();
}

void loop() {
  o2VoltageSensor->loop();
  heVoltageSensor->loop();
  atmosphericPressureSensor->loop();
  temperatureSensor->loop();

  float o2SensorVoltage = o2VoltageSensor->read();
  float heSensorVoltage = heVoltageSensor->read();
  float currentAtmosphericPressure = atmosphericPressureSensor->read();
  float temperature = temperatureSensor->read();

  ///

  float atmosphericO2Percentage = getAtmosphericO2Percentage(currentAtmosphericPressure);
  float fractionHe = getHeFractionFromVoltage(heSensorVoltage);
  float fractionO2 = getO2FractionFromVoltage(o2SensorVoltage, atmosphericO2Percentage);

  float percentageHe = fractionHe * 100;
  float percentageO2 = fractionO2 * 100;

  display.showDisplayGasInformation(o2SensorVoltage, percentageO2);

  if (callibrateSignalReceived) {
    buzzer.beep();
    calibrateSensors();
    display.showDisplayCalibratingMessage();
    delay(1000);

    callibrateSignalReceived = false;
  }

  if (deviceConnected) {
    rtcTime.serialPrintDateTime();

    // Serialize data
    char output[200];
    JsonDocument doc;

    doc["percentageO2"] = percentageO2;
    doc["percentageHe"] = percentageHe;
    doc["o2SensorVoltage"] = o2SensorVoltage;
    doc["heSensorVoltage"] = heSensorVoltage;
    doc["atmosphericPressure"] = currentAtmosphericPressure;
    doc["temperature"] = temperature;

    serializeJson(doc, output);

    // Send data
    txCharacteristic->setValue(output);
    txCharacteristic->notify();

    // Print data
    serializeJsonPretty(doc, Serial);
    Serial.println();

    delay(100);// Avoid BLE data congestion
  }

  // Disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    buzzer.beep();
    display.showDisplayMessage("Disconnected!");
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

    buzzer.beep();
    display.showDisplayMessage("Connected!");
    delay(500);
  }
}
