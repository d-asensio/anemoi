#include <Arduino.h>
#include <Wire.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include <DateTimeModule.h>
#include <DisplayModule.h>
#include <GasSensorsModule.h>
#include <AtmosphericSensorModule.h>
#include <BuzzerModule.h>

#define SERVICE_UUID "9243e98a-314c-42b2-a4fc-c23d54f0f271"

#define O2_PERCENTAGE_CHARACTERISTIC_UUID "44aa55a3-564f-4d9a-b20e-6636e0c43dfc"
#define ATMOSPHERIC_PRESSURE_CHARACTERISTIC_UUID "68848368-6d91-49f9-9a5f-fed73463c9f6"
#define TEMPERATURE_PRESSURE_CHARACTERISTIC_UUID "a9bac333-e37c-42a9-8abc-9b07350e189d"
#define CALIBRATE_SIGNAL_CHARACTERISTIC_UUID "8d07c070-b5e0-4859-bc71-88b425e040c0"

#define ATMOSPHERIC_O2_PERCENTAGE_AT_SEA_LEVEL 0.209
#define ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL 1013.25 // in millibars

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

float getAtmosphericO2Percentage(float currentAtmosphericPressure) {
  return (ATMOSPHERIC_O2_PERCENTAGE_AT_SEA_LEVEL * currentAtmosphericPressure) / ATMOSPHERIC_PRESSURE_AT_SEA_LEVEL;
}

float getO2FractionFromVoltage(float currentCellVoltage, float atmosphericO2Percentage) {
  return atmosphericO2Percentage * currentCellVoltage / calibrationO2SensorVoltage;
}

void calibrateO2Sensor() {
  calibrationO2SensorVoltage = readO2SensorVoltage();
  calibrationAtmosphericPressure = readAtmosphericPressure();
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

  initDateTimeModule();
  initDisplayModule();
  initGasSensorsModule();
  initAtmosphericSensorModule();
  initBuzzerModule();

  calibrateO2Sensor();
  showDisplayCalibratingMessage();
  buzzerBeep();
}

void loop() {
  atmosphericSensorModuleLoop();

  float o2SensorVoltage = readO2SensorVoltage();
  float currentAtmosphericPressure = readAtmosphericPressure();
  float atmosphericO2Percentage = getAtmosphericO2Percentage(currentAtmosphericPressure);
  float fractionO2 = getO2FractionFromVoltage(o2SensorVoltage, atmosphericO2Percentage);

  float percentageO2 = fractionO2 * 100;

  float temperature = readTemperature();

  showDisplayGasInformation(o2SensorVoltage, percentageO2);

  if (callibrateSignalReceived) {
    buzzerBeep();
    calibrateO2Sensor();
    showDisplayCalibratingMessage();
    delay(1000);

    callibrateSignalReceived = false;
  }

  if (deviceConnected) {
    serialPrintDateTime();

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
}
