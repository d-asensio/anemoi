import styled from "styled-components/native";
import React from "react";

const Wrapper = styled.View`
  width: 100%;
  flex-direction: row;
  align-items: center;
  justify-content: space-between;
  padding-right: 32px;
  padding-left: 32px;
  margin-bottom: 16px;
`
const Symbol = styled.Text`
    font-size: 48px;
    font-weight: bold;
`
const Value = styled.Text`
    font-size: 40px;
    font-weight: normal;
`

export const MetricIndicator = ({symbol, value, unit}: { symbol: string, value: number, unit: string }) => (
  <Wrapper>
    <Symbol>
      {symbol}
    </Symbol>
    <Value>
      {value.toFixed(2)} {unit}
    </Value>
  </Wrapper>
)