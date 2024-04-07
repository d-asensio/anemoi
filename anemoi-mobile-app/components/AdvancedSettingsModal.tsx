import React, {FC, useCallback} from "react";
import styled from 'styled-components/native';
import {useMMKVObject} from "react-native-mmkv";
import {FlatList, ListRenderItemInfo, Text, View} from "react-native"

import {SecondaryButton} from "./SecondaryButton";
import type {AnalyzerData} from "../useBLE";
import {MetricIndicator} from "./MetricIndicator";
import {Section} from "./Section";
import {IconButton} from "./IconButton";
import {Save, CircleX} from "lucide-react-native";

type DeviceModalProps = {
  open: boolean;
  data: AnalyzerData;
  onClosePressed: () => void;
};

const Wrapper = styled.Modal`
  flex: 1;
  background-color: #f2f2f2;
`;

const Content = styled.SafeAreaView`
  flex: 1;
  background-color: #f2f2f2;
  justify-content: space-between;
`;

const List = styled(FlatList<Measurement>)`
    margin: 24px 24px 16px;
    border: 1px solid transparent;
    border-top-color: #c0d6df;
    border-bottom-color: #c0d6df;
`;

const MeasurementItemWrapper = styled.View`
    margin-top: 8px;
    margin-bottom: 8px;
    padding: 8px 16px;
    
    border-radius: 16px;
    
    background: #00303D;
`

const MeasurementValues = styled.View`
    flex-direction: row;
    justify-content: space-between;
    margin-bottom: 8px;
`

const MeasurementValue = styled.Text`
    color: #FFF;
    font-size: 24px;
`

const MeasurementItemDate = styled.Text`
    font-size: 16px;
    color: #c0d6df;
`

const HistoryButtonsWrapper = styled.View`
    flex-direction: row;
    justify-content: center;
    align-items: center;
    gap: 8px;
`

const IndicatorsWrapper = styled.View`
    padding-top: 42px;
    padding-right: 24px;
    padding-left: 24px;
`;

type Measurement = {
  date: string
  o2SensorVoltage: number
  heSensorVoltage: number
}

const MeasurementListItem: FC<Measurement> = ({ date, o2SensorVoltage, heSensorVoltage }) => {
  return (
    <MeasurementItemWrapper>
      <MeasurementValues>
        <MeasurementValue>
          O2: {o2SensorVoltage.toFixed(2)} mV
        </MeasurementValue>
        <MeasurementValue>
          He: {heSensorVoltage.toFixed(2)} mV
        </MeasurementValue>
      </MeasurementValues>
      <MeasurementItemDate>
        {new Date(date).toUTCString()}
      </MeasurementItemDate>
    </MeasurementItemWrapper>
  );
};

const renderMeasurementListItem = ({ item: measurement }: ListRenderItemInfo<Measurement>) => (
  <MeasurementListItem {...measurement} />
)

export const AdvancedSettingsModal: FC<DeviceModalProps> = ({open, data: {o2SensorVoltage, heSensorVoltage}, onClosePressed }: DeviceModalProps) => {
  const [sensorMeasurementHistory, setSensorMeasurementHistory] = useMMKVObject<Measurement[]>("sensor-measurements")

  const handleSaveMeasurePress = useCallback(() => {
    setSensorMeasurementHistory([
      {
        date: new Date().toDateString(),
        o2SensorVoltage,
        heSensorVoltage
      },
      ...(sensorMeasurementHistory ?? [])
    ])
  }, [setSensorMeasurementHistory, sensorMeasurementHistory])

  const handleClearHistoryPress = useCallback(() => {
    setSensorMeasurementHistory([])
  }, [setSensorMeasurementHistory])

  return (
    <Wrapper animationType="slide" transparent={false} visible={open}>
      <Content>
        <IndicatorsWrapper>
          <MetricIndicator
            symbol="vO2"
            value={o2SensorVoltage}
            unit="mV"
          />
          <MetricIndicator
            symbol="vHe"
            value={heSensorVoltage}
            unit="mV"
          />
        </IndicatorsWrapper>
        <HistoryButtonsWrapper>
          <IconButton onPress={handleSaveMeasurePress}>
            <Save color="#FFF" size={24} />
          </IconButton>
          <IconButton onPress={handleClearHistoryPress}>
            <CircleX color="#FFF" size={24} />
          </IconButton>
        </HistoryButtonsWrapper>
        <List
          data={sensorMeasurementHistory}
          renderItem={renderMeasurementListItem}
        />
        <Section>
          <SecondaryButton onPress={onClosePressed}>
            Back
          </SecondaryButton>
        </Section>
      </Content>
    </Wrapper>
  );
};
