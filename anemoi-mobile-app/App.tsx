import React, {useState} from "react";
import {SafeAreaView, StyleSheet,} from "react-native";
import DeviceModal from "./DeviceConnectionModal";
import useBLE from "./useBLE";
import {Button} from "./components/Button";
import styled from "styled-components/native";
import {StartScreen} from "./screens/StartScreen";
import {SecondaryButton} from "./components/SecondaryButton";
import {LiveDataScreen} from "./screens/LiveDataScreen";

const Section = styled.View`
    padding-right: 24px;
    padding-left: 24px;
`;


const App = () => {
  const {
    scanForPeripherals,
    connectToDevice,
    disconnectFromDevice,
    allDevices,
    connectedDevice,
    sendCalibrateSignal,
    data
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
        <LiveDataScreen data={data}/>
      ) : (
        <StartScreen/>
      )}
      <Section>
        {connectedDevice
          ? (
            <SecondaryButton onPress={sendCalibrateSignal}>
              {"Calibarte Sensors"}
            </SecondaryButton>
          )
          : (
            <Button onPress={openModal}>
              {"Connect"}
            </Button>
          )
        }
      </Section>
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
    justifyContent: "space-between",
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
  noConnectedDevicesText: {
    fontSize: 20,
    textAlign: "center",
    marginHorizontal: 40,
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
