import React, {FC} from "react";
import styled from 'styled-components/native';

import {SecondaryButton} from "./SecondaryButton";
import type {AnalyzerData} from "../useBLE";
import {MetricIndicator} from "./MetricIndicator";
import {Section} from "./Section";

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

const List = styled.View`
  margin-top: 24px;
  border: 1px solid transparent;
  border-bottom-color: #c0d6df;
`;

const IndicatorsWrapper = styled.View`
    padding-top: 42px;
    padding-right: 24px;
    padding-left: 24px;
`;

export const AdvancedSettingsModal: FC<DeviceModalProps> = ({ open, data, onClosePressed }: DeviceModalProps) => {
  return (
    <Wrapper animationType="slide" transparent={false} visible={open}>
      <Content>
        <IndicatorsWrapper>
          <MetricIndicator
            symbol="vO2"
            value={data.o2SensorVoltage}
            unit="mV"
          />
          <MetricIndicator
            symbol="vHe"
            value={data.heSensorVoltage}
            unit="mV"
          />
        </IndicatorsWrapper>
        <Section>
          <List>

          </List>
        </Section>
        <Section>
          <SecondaryButton onPress={() => {}}>
            Save Measure
          </SecondaryButton>
          <SecondaryButton onPress={onClosePressed}>
            Back
          </SecondaryButton>
        </Section>
      </Content>
    </Wrapper>
  );
};
