/* eslint-disable no-bitwise */
import {useMemo, useState} from "react";
import {BleError, BleManager, Characteristic, Device,} from "react-native-ble-plx";

import base64 from "react-native-base64";

const ANEMOI_SERVICE_UUID = "9243e98a-314c-42b2-a4fc-c23d54f0f271";
const RX_CHARACTERISTIC_UUID = "44aa55a3-564f-4d9a-b20e-6636e0c43dfc";
const CALIBRATE_CHARACTERISTIC_UUID = "8d07c070-b5e0-4859-bc71-88b425e040c0"

export interface AnalyzerData {
  percentageO2: number;
  percentageHe: number;
  o2SensorVoltage: number;
  heSensorVoltage: number;
  atmosphericPressure: number;
  temperature: number;
}

interface BluetoothLowEnergyApi {
  scanForPeripherals(): void;
  connectToDevice: (deviceId: Device) => Promise<void>;
  sendCalibrateSignal: () => Promise<void>;
  disconnectFromDevice: () => void;
  connectedDevice: Device | null;
  allDevices: Device[];
  data: AnalyzerData
}

function useBLE(): BluetoothLowEnergyApi {
  const bleManager = useMemo(() => new BleManager(), []);
  const [allDevices, setAllDevices] = useState<Device[]>([]);
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [data, setData] = useState<AnalyzerData>({
    percentageO2: 0,
    percentageHe: 0,
    o2SensorVoltage: 0,
    heSensorVoltage: 0,
    atmosphericPressure: 0,
    temperature: 0,
  });

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

  const onDataReceived = (decodedData: string) => {
    setData(JSON.parse(decodedData));
  };

  const startStreamingData = async (device: Device) => {
    if (!device) {
      console.log("No Device Connected (starting data stream)");
      return;
    }

    device.monitorCharacteristicForService(
      ANEMOI_SERVICE_UUID,
      RX_CHARACTERISTIC_UUID,
      createCharacteristicHandler(onDataReceived)
    );
  };

  return {
    scanForPeripherals,
    connectToDevice,
    allDevices,
    connectedDevice,
    disconnectFromDevice,
    sendCalibrateSignal,
    data
  };
}

export default useBLE;
