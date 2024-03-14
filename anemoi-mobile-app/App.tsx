import React, {useState} from "react";
import {
  SafeAreaView,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from "react-native";
import DeviceModal from "./DeviceConnectionModal";
import {PulseIndicator} from "./PulseIndicator";
import useBLE from "./useBLE";

const App = () => {
  const {
    scanForPeripherals,
    connectToDevice,
    disconnectFromDevice,
    allDevices,
    connectedDevice,
    sendCalibrateSignal,
    percentageO2,
    atmosphericPressure,
    temperature
  } = useBLE();
  const [isModalVisible, setIsModalVisible] = useState<boolean>(false);

  const hideModal = () => {
    setIsModalVisible(false);
  };

  const openModal = async () => {
    scanForPeripherals();
    setIsModalVisible(true);
  };

  return (
    <SafeAreaView style={styles.container}>
      {connectedDevice ? (
        <View style={styles.dataView}>
          <PulseIndicator/>
          <View style={styles.dataPointView}>
            <Text style={styles.dataPointValue}>{percentageO2}</Text>
            <Text style={styles.dataPointUnits}>% O2</Text>
          </View>
          <View style={styles.dataPointView}>
            <Text style={styles.dataPointValue}>{atmosphericPressure}</Text>
            <Text style={styles.dataPointUnits}>mbar</Text>
          </View>
          <View style={styles.dataPointView}>
            <Text style={styles.dataPointValue}>{temperature}</Text>
            <Text style={styles.dataPointUnits}>℃</Text>
          </View>
          <TouchableOpacity
            onPress={sendCalibrateSignal}
            style={styles.calibrateButton}
          >
            <Text style={styles.calibrateButtonText}>
              Calibrate
            </Text>
          </TouchableOpacity>
        </View>
      ) : (
        <View style={styles.noConnectedDevicesView}>
          <Text style={styles.noConnectedDevicesText}>
            Please connect to the analyzer to see its data
          </Text>
        </View>
      )}
      <TouchableOpacity
        onPress={connectedDevice ? disconnectFromDevice : openModal}
        style={styles.ctaButton}
      >
        <Text style={styles.ctaButtonText}>
          {connectedDevice ? "Disconnect" : "Connect"}
        </Text>
      </TouchableOpacity>
      <DeviceModal
        closeModal={hideModal}
        visible={isModalVisible}
        connectToPeripheral={connectToDevice}
        devices={allDevices}
      />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  dataView: {
    flex: 1,
    justifyContent: "flex-start",
    alignItems: "center"
  },
  dataPointView: {
    flexDirection: "row",
    alignItems: "baseline",
    color: "#2d2a32",
    marginBottom: 15
  },
  dataPointValue: {
    fontSize: 50,
    fontWeight: "bold",
    textAlign: "center",
    marginHorizontal: 5,
    color: "#2d2a32",
  },
  dataPointUnits: {
    fontSize: 20
  },
  noConnectedDevicesView: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  noConnectedDevicesText: {
    fontSize: 20,
    textAlign: "center",
    marginHorizontal: 40,
    color: "#2d2a32",
  },
  ctaButton: {
    backgroundColor: "#f9dc5c",
    justifyContent: "center",
    alignItems: "center",
    height: 50,
    marginHorizontal: 20,
    marginBottom: 5,
    borderRadius: 8,
  },
  ctaButtonText: {
    fontSize: 18,
    fontWeight: "bold",
    color: "#2d2a32",
  },
  calibrateButton: {
    borderColor: "#2d2a32",
    borderWidth: 1,
    justifyContent: "center",
    alignItems: "center",
    height: 50,
    paddingHorizontal: 20,
    borderRadius: 8,
  },
  calibrateButtonText: {
    fontSize: 18,
    fontWeight: "bold",
    color: "#2d2a32",
  },
});

export default App;
