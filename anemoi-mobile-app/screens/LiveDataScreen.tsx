import type {AnalyzerData} from "../useBLE";
import {MetricIndicator} from "../components/MetricIndicator";
import {SmallMetricIndicator} from "../components/SmallMetricIndicator";
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
      <MetricIndicator
        symbol="O2"
        value={data.percentageO2}
        unit="%"
      />
      <MetricIndicator
        symbol="He"
        value={data.percentageHe}
        unit="%"
      />
      <ConnectedDeviceBadge
        label="Connected"
        deviceName="Anemoi Mini"
      />
      <SmallMetricIndicator
        symbol="Temperature"
        value={data.temperature}
        unit="℃"
      />
      <SmallMetricIndicator
        symbol="Relative Humidity"
        value={42}
        precision={0}
        unit="%"
      />
      <SmallMetricIndicator
        symbol="Atm. Pressure"
        value={data.atmosphericPressure / 1000}
        unit="bar"
      />
    </Wrapper>
  );
}