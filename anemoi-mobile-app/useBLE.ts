/* eslint-disable no-bitwise */
import {useMemo, useState} from "react";
import {BleError, BleManager, Characteristic, Device,} from "react-native-ble-plx";

import base64 from "react-native-base64";

const ANEMOI_SERVICE_UUID = "9243e98a-314c-42b2-a4fc-c23d54f0f271";
const HE_VOLTAGE_CHARACTERISTIC_UUID = "868b31f7-4c08-4d5f-b0fa-9e5151b19f5c";
const HE_PERCENTAGE_CHARACTERISTIC_UUID = "61de16b4-dbcf-491a-8ed6-5ba35a9c60e7";
const O2_VOLTAGE_CHARACTERISTIC_UUID = "55920ac9-31d3-45d3-8d4e-89566077fbd9";
const O2_PERCENTAGE_CHARACTERISTIC_UUID = "44aa55a3-564f-4d9a-b20e-6636e0c43dfc";
const ATMOSPHERIC_PRESSURE_CHARACTERISTIC_UUID = "68848368-6d91-49f9-9a5f-fed73463c9f6";
const TEMPERATURE_CHARACTERISTIC_UUID = "a9bac333-e37c-42a9-8abc-9b07350e189d";
const CALIBRATE_CHARACTERISTIC_UUID = "8d07c070-b5e0-4859-bc71-88b425e040c0"

interface BluetoothLowEnergyApi {
  scanForPeripherals(): void;
  connectToDevice: (deviceId: Device) => Promise<void>;
  sendCalibrateSignal: () => Promise<void>;
  disconnectFromDevice: () => void;
  connectedDevice: Device | null;
  allDevices: Device[];
  percentageO2: number;
  atmosphericPressure: number;
  temperature: number;
  voltageO2: number;
  percentageHe: number;
  voltageHe: number;
}

function useBLE(): BluetoothLowEnergyApi {
  const bleManager = useMemo(() => new BleManager(), []);
  const [allDevices, setAllDevices] = useState<Device[]>([]);
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [voltageHe, setVoltageHe] = useState<number>(0);
  const [percentageHe, setPercentageHe] = useState<number>(0);
  const [voltageO2, setVoltageO2] = useState<number>(0);
  const [percentageO2, setPercentageO2] = useState<number>(0);
  const [atmosphericPressure, setAtmosphericPressure] = useState<number>(0);
  const [temperature, setTemperature] = useState<number>(0);

  const isDuplicateDevice = (devices: Device[], nextDevice: Device) =>
    devices.findIndex((device) => nextDevice.id === device.id) > -1;

  const scanForPeripherals = () =>
    bleManager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.log(error);
      }
      if (device && device.name?.includes("Anemoi")) {
        setAllDevices((prevState: Device[]) => {
          if (!isDuplicateDevice(prevState, device)) {
            return [...prevState, device];
          }
          return prevState;
        });
      }
    });

  const connectToDevice = async (device: Device) => {
    try {
      const deviceConnection = await bleManager.connectToDevice(device.id);
      setConnectedDevice(deviceConnection);
      await deviceConnection.discoverAllServicesAndCharacteristics();
      bleManager.stopDeviceScan();
      await startStreamingData(deviceConnection);
    } catch (e) {
      console.log("FAILED TO CONNECT", e);
    }
  };

  const disconnectFromDevice = async () => {
    if (connectedDevice) {
      try {
        await bleManager.cancelDeviceConnection(connectedDevice.id);
      } catch {
        console.log("Device already disconnected!")
      }

      setConnectedDevice(null);

      setPercentageO2(0);
      setAtmosphericPressure(0);
      setTemperature(0);
    }
  };

  const createCharacteristicHandler = (handlerFn: (decodedData: string) => void) => (
    error: BleError | null,
    characteristic: Characteristic | null
  ) => {
    if (error) {
      console.log(error);
      return -1;
    } else if (!characteristic?.value) {
      console.log("No Data was receieved");
      return -1;
    }

    const rawData = base64.decode(characteristic.value).trim();

    handlerFn(rawData)
  };

  const sendCalibrateSignal = async () => {
    if (!connectedDevice) {
      console.log("No Device Connected (sending calibrate signal)");
      return;
    }

    await connectedDevice.writeCharacteristicWithResponseForService(
      ANEMOI_SERVICE_UUID,
      CALIBRATE_CHARACTERISTIC_UUID,
      base64.encode("CAL")
    )
  }

  const onVoltageHeUpdate = (decodedData: string) => {
    setVoltageHe(parseFloat(decodedData));
  };

  const onPercentageHeUpdate = (decodedData: string) => {
    setPercentageHe(parseFloat(decodedData));
  };

  const onVoltageO2Update = (decodedData: string) => {
    setVoltageO2(parseFloat(decodedData));
  };

  const onPercentageO2Update = (decodedData: string) => {
    setPercentageO2(parseFloat(decodedData));
  };

  const onAtmosphericPressureUpdate = (decodedData: string) => {
    setAtmosphericPressure(parseFloat(decodedData));
  };

  const onTemperatureUpdate = (decodedData: string) => {
    setTemperature(parseFloat(decodedData));
  };

  const startStreamingData = async (device: Device) => {
    if (!device) {
      console.log("No Device Connected (starting data stream)");
      return;
    }

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      HE_PERCENTAGE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onPercentageHeUpdate)
    );

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      HE_VOLTAGE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onVoltageHeUpdate)
    );

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      O2_VOLTAGE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onVoltageO2Update)
    );

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      O2_PERCENTAGE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onPercentageO2Update)
    );

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      ATMOSPHERIC_PRESSURE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onAtmosphericPressureUpdate)
    );

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      TEMPERATURE_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onTemperatureUpdate)
    );
  };

  return {
    scanForPeripherals,
    connectToDevice,
    allDevices,
    connectedDevice,
    disconnectFromDevice,
    sendCalibrateSignal,
    voltageHe,
    percentageHe,
    voltageO2,
    percentageO2,
    atmosphericPressure,
    temperature
  };
}

export default useBLE;
