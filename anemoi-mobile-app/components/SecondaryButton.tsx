import type {GestureResponderEvent} from "react-native";
import React from 'react';
import styled from 'styled-components/native';

const Wrapper = styled.TouchableOpacity`
    display: flex;
    justify-content: center;
    width: 100%;
    height: 64px;
    padding: 16px;
    border-radius: 9999px;
    background: #FFF;
    border: #004052 solid 3px;
`;

const Text = styled.Text`
    font-size: 20px;
    font-weight: bold;
    text-align: center;
    color: #004052;
`;

export const SecondaryButton = ({children, onPress}: { children: string, onPress: (e: GestureResponderEvent) => void }) => (
  <Wrapper onPress={onPress}>
    <Text>{children}</Text>
  </Wrapper>
);