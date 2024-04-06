import React, {useState} from "react";
import styled from "styled-components/native";

import useBLE from "./useBLE";

import DeviceModal from "./components/DeviceConnectionModal";
import {Button} from "./components/Button";
import {StartScreen} from "./screens/StartScreen";
import {SecondaryButton} from "./components/SecondaryButton";
import {LiveDataScreen} from "./screens/LiveDataScreen";
import {AdvancedSettingsModal} from "./components/AdvancedSettingsModal";
import {GhostButton} from "./components/GhostButton";
import {Section} from "./components/Section";

const Wrapper = styled.SafeAreaView`
    flex: 1;
    justify-content: space-between;
    background: #f2f2f2;
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
  const [isDevicesModalVisible, setIsDevicesModalVisible] = useState<boolean>(false);
  const [isAdvancedConfigModalVisible, setIsAdvancedConfigVisible] = useState<boolean>(false);

  const hideDeviceModal = () => {
    setIsDevicesModalVisible(false);
  };

  const openDevicesModal = async () => {
    scanForPeripherals();
    setIsDevicesModalVisible(true);
  };

  const hideAdvancedConfigModal = () => {
    setIsAdvancedConfigVisible(false);
  };

  const openAdvancedConfigModal = async () => {
    setIsAdvancedConfigVisible(true);
  };

  return (
    <Wrapper>
      {connectedDevice ? (
        <LiveDataScreen data={data}/>
      ) : (
        <StartScreen/>
      )}
      <Section>
        {connectedDevice
          ? (
            <>
              <GhostButton onPress={openAdvancedConfigModal}>
                {"Advanced settings"}
              </GhostButton>
              <SecondaryButton onPress={sendCalibrateSignal}>
                {"Calibarte Sensors"}
              </SecondaryButton>
            </>
          )
          : (
            <Button onPress={openDevicesModal}>
              {"Connect"}
            </Button>
          )
        }
      </Section>
      <DeviceModal
        closeModal={hideDeviceModal}
        visible={isDevicesModalVisible}
        connectToPeripheral={connectToDevice}
        devices={allDevices}
      />
      <AdvancedSettingsModal
        onClosePressed={hideAdvancedConfigModal}
        open={isAdvancedConfigModalVisible}
        data={data}
      />
    </Wrapper>
  );
};

export default App;
