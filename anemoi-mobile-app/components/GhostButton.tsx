import type {GestureResponderEvent} from "react-native";
import React from 'react';
import styled from 'styled-components/native';

const Wrapper = styled.TouchableOpacity`
    display: flex;
    justify-content: center;
    width: 100%;
    height: 64px;
    padding: 16px;
    margin-bottom: 8px;
`;

const Text = styled.Text`
    font-size: 20px;
    font-weight: bold;
    text-align: center;
    color: #004052;
    text-decoration: underline;
    text-decoration-color: #004052;
`;

export const GhostButton = ({children, onPress}: { children: string, onPress: (e: GestureResponderEvent) => void }) => (
  <Wrapper onPress={onPress}>
    <Text>{children}</Text>
  </Wrapper>
);