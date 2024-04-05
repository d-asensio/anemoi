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
    background: #004052;
`;

const Text = styled.Text`
    font-size: 20px;
    font-weight: bold;
    text-align: center;
    color: #FFF;
`;

export const Button = ({children, onPress}: { children: string, onPress: (e: GestureResponderEvent) => void }) => (
  <Wrapper onPress={onPress}>
    <Text>{children}</Text>
  </Wrapper>
);