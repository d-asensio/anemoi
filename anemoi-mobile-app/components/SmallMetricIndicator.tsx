import styled from "styled-components/native";
import React from "react";

const Wrapper = styled.View`
  width: 100%;
  flex-direction: row;
  justify-content: space-between;
  align-items: baseline;
  padding-right: 16px;
  padding-left: 16px;
  margin-bottom: 8px;
`
const Symbol = styled.Text`
    font-size: 24px;
    font-weight: bold;
`
const Value = styled.Text`
    font-size: 32px;
    font-weight: normal;
`

export const SmallMetricIndicator = ({symbol, value, unit, precision = 2}: { symbol: string, value: number, precision?: number, unit: string }) => (
  <Wrapper>
    <Symbol>
      {symbol}
    </Symbol>
    <Value>
      {value.toFixed(precision)} {unit}
    </Value>
  </Wrapper>
)