import styled from "styled-components/native";
import {Logo} from "../components/Logo";
import React from "react";

const Wrapper = styled.View`
  align-items: center;
`;
const WelcomeText = styled.Text`
    font-size: 32px;
    font-weight: normal;
    text-align: center;
    color: #00303D;
`;

export function StartScreen() {
  return (
    <Wrapper>
      <Logo/>
      <WelcomeText>
        Welcome to Anemoi
      </WelcomeText>
    </Wrapper>
  );
}