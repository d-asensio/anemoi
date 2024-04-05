import type {AnalyzerData} from "../useBLE";
import {GasPercentageIndicator} from "../components/GasPercentageIndicator";
import {MetricIndicator} from "../components/MetricIndicator";
import React from "react";
import styled from "styled-components/native";
import {ConnectedDeviceBadge} from "../components/ConnectedDeviceBadge";

const Wrapper = styled.View`
    padding-top: 42px;
    padding-right: 24px;
    padding-left: 24px;
`;


export function LiveDataScreen({data}: { data: AnalyzerData }) {
  return (
    <Wrapper>
      <GasPercentageIndicator
        symbol="O2"
        value={data.percentageO2}
      />
      <GasPercentageIndicator
        symbol="He"
        value={data.percentageHe}
      />
      <ConnectedDeviceBadge
        label="Connected"
        deviceName="Anemoi Mini"
      />
      <MetricIndicator
        symbol="Temperature"
        value={data.temperature}
        unit="℃"
      />
      <MetricIndicator
        symbol="Relative Humidity"
        value={42}
        precision={0}
        unit="%"
      />
      <MetricIndicator
        symbol="Atm. Pressure"
        value={data.atmosphericPressure / 1000}
        unit="bar"
      />
    </Wrapper>
  );
}