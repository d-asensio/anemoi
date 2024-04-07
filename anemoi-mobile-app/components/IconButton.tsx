import type {GestureResponderEvent} from "react-native";
import React, {PropsWithChildren} from 'react';
import styled from 'styled-components/native';

const Wrapper = styled.TouchableOpacity`
    display: flex;
    justify-content: center;
    align-items: center;
    width: 64px;
    height: 64px;
    padding: 16px;
    border-radius: 9999px;
    background: #004052;
`;

export const IconButton = ({children, onPress}: PropsWithChildren<{ onPress: (e: GestureResponderEvent) => void }>) => (
  <Wrapper onPress={onPress}>
    {children}
  </Wrapper>
);