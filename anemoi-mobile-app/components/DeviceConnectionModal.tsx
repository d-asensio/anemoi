import type { Device } from "react-native-ble-plx";
import React, { FC, useCallback } from "react";
import {
  FlatList,
  ListRenderItemInfo
} from "react-native";
import styled from 'styled-components/native';
import { Signal } from 'lucide-react-native';

type DeviceModalListItemProps = {
  item: ListRenderItemInfo<Device>;
  connectToPeripheral: (device: Device) => void;
  closeModal: () => void;
};

type DeviceModalProps = {
  devices: Device[];
  visible: boolean;
  connectToPeripheral: (device: Device) => void;
  closeModal: () => void;
};

const ItemWrapper = styled.TouchableOpacity`
  flex-direction: row;
  justify-content: space-between;
  padding-horizontal: 8px;
  padding-vertical: 16px;

  border: 1px solid transparent;
  border-top-color: #c0d6df;
`;

const ItemText = styled.Text`
  font-size: 22px;
  font-weight: normal;
  color: #00303D;
`;

const Wrapper = styled.Modal`
  flex: 1;
  background-color: #f2f2f2;
`;

const Content = styled.SafeAreaView`
  flex: 1;
  background-color: #f2f2f2;
`;

const List = styled.View`
  margin-top: 24px;
  margin-horizontal: 24px;
  border: 1px solid transparent;
  border-bottom-color: #c0d6df;
`;

const Title = styled.Text`
    margin-top: 42px;
    font-size: 32px;
    font-weight: normal;
    text-align: center;
    color: #00303D;
`;

const DeviceModalListItem: FC<DeviceModalListItemProps> = (props) => {
  const { item, connectToPeripheral, closeModal } = props;

  const connectAndCloseModal = useCallback(() => {
    connectToPeripheral(item.item);
    closeModal();
  }, [closeModal, connectToPeripheral, item.item]);

  return (
    <ItemWrapper onPress={connectAndCloseModal}>
      <ItemText>{item.item.name}</ItemText>
      <Signal color="#7BE4A5" size={24} />
    </ItemWrapper>
  );
};

const DeviceConnectionModal: FC<DeviceModalProps> = (props) => {
  const { devices, visible, connectToPeripheral, closeModal } = props;

  const renderDeviceModalListItem = useCallback(
    (item: ListRenderItemInfo<Device>) => {
      return (
        <DeviceModalListItem
          item={item}
          connectToPeripheral={connectToPeripheral}
          closeModal={closeModal}
        />
      );
    },
    [closeModal, connectToPeripheral]
  );

  return (
    <Wrapper animationType="slide" transparent={false} visible={visible}>
      <Content>
        <Title>Select a device</Title>
        <List>
          <FlatList data={devices} renderItem={renderDeviceModalListItem} />
        </List>
      </Content>
    </Wrapper>
  );
};

export default DeviceConnectionModal;
