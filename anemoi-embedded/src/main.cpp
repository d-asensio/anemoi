#include <Arduino.h>
#include <Wire.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include <DateTimeModule.h>
#include <BuzzerModule.h>

#include <Adafruit_ADS1X15.h>
#include <Adafruit_BMP280.h>

#include "ISensor.h"
#include "HeVoltageSensor.h"
#include "O2VoltageSensor.h"
#include "AtmosphericPressureSensor.h"
#include "TemperatureSensor.h"

#include "Display.h"

#define SERVICE_UUID "9243e98a-314c-42b2-a4fc-c23d54f0f271"

#define HE_VOLTAGE_CHARACTERISTIC_UUID "868b31f7-4c08-4d5f-b0fa-9e5151b19f5c"
#define HE_PERCENTAGE_CHARACTERISTIC_UUID "61de16b4-dbcf-491a-8ed6-5ba35a9c60e7"
#define O2_VOLTAGE_CHARACTERISTIC_UUID "55920ac9-31d3-45d3-8d4e-89566077fbd9"
#define O2_PERCENTAGE_CHARACTERISTIC_UUID "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"
#define CALIBRATE_SIGNAL_CHARACTERISTIC_UUID "8d07c070-b5e0-4859-bc71-88b425e040c0"

#define ATMOSPHERIC_O2_PERCENTAGE_AT_SEA_LEVEL 0.209
#define ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL 1013.25 // in millibars

///
logging::Logger logger;
Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;


ISensor *heVoltageSensor = new HeVoltageSensor(&logger, &ads);
ISensor *o2VoltageSensor = new O2VoltageSensor(&logger, &ads);
ISensor *atmosphericPressureSensor = new AtmosphericPressureSensor(&logger, &bmp);
ISensor *temperatureSensor = new TemperatureSensor(&logger, &bmp);

Display display(&logger);
///

float calibrationO2SensorVoltage = 0;
float calibrationHeSensorVoltage = 0;
float calibrationAtmosphericPressure = 0;

BLEServer *pServer = nullptr;
BLECharacteristic *voltageHeCharacteristic;
BLECharacteristic *percentageHeCharacteristic;
BLECharacteristic *voltageO2Characteristic;
BLECharacteristic *percentageO2Characteristic;
BLECharacteristic *callibrateSignalCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

bool callibrateSignalReceived = false;

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


  voltageHeCharacteristic = pService->createCharacteristic(
          HE_VOLTAGE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  voltageHeCharacteristic->addDescriptor(new BLE2902());


  percentageHeCharacteristic = pService->createCharacteristic(
          HE_PERCENTAGE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  percentageHeCharacteristic->addDescriptor(new BLE2902());


  voltageO2Characteristic = pService->createCharacteristic(
          O2_VOLTAGE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  voltageO2Characteristic->addDescriptor(new BLE2902());

  
  percentageO2Characteristic = pService->createCharacteristic(
          O2_PERCENTAGE_CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_NOTIFY);
  percentageO2Characteristic->addDescriptor(new BLE2902());

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

  initBLEConnection();
  initDateTimeModule();
  initBuzzerModule();

  calibrateSensors();
  display.showDisplayCalibratingMessage();
  buzzerBeep();
}

void loop() {
  o2VoltageSensor->loop();
  heVoltageSensor->loop();
  atmosphericPressureSensor->loop();

  float o2SensorVoltage = o2VoltageSensor->read();
  float heSensorVoltage = heVoltageSensor->read();
  float currentAtmosphericPressure = atmosphericPressureSensor->read();

  ///

  float atmosphericO2Percentage = getAtmosphericO2Percentage(currentAtmosphericPressure);
  float fractionHe = getHeFractionFromVoltage(heSensorVoltage);
  float fractionO2 = getO2FractionFromVoltage(o2SensorVoltage, atmosphericO2Percentage);

  float percentageHe = fractionHe * 100;
  float percentageO2 = fractionO2 * 100;

  display.showDisplayGasInformation(o2SensorVoltage, percentageO2);

  if (callibrateSignalReceived) {
    buzzerBeep();
    calibrateSensors();
    display.showDisplayCalibratingMessage();
    delay(1000);

    callibrateSignalReceived = false;
  }

  if (deviceConnected) {
    serialPrintDateTime();

    // Send He voltage data
    char voltageHeTmp[50];
    dtostrf(heSensorVoltage, 6, 2, voltageHeTmp);
    voltageHeCharacteristic->setValue(voltageHeTmp);
    voltageHeCharacteristic->notify();

    // Send He percentage data
    char percentageHeTmp[50];
    dtostrf(percentageHe, 6, 2, percentageHeTmp);
    percentageHeCharacteristic->setValue(percentageHeTmp);
    percentageHeCharacteristic->notify();

    // Send O2 voltage data
    char voltageO2Tmp[50];
    dtostrf(o2SensorVoltage, 6, 2, voltageO2Tmp);
    voltageO2Characteristic->setValue(voltageO2Tmp);
    voltageO2Characteristic->notify();

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

    buzzerBeep();
    display.showDisplayMessage("Connected!");
    delay(500);
  }
}
