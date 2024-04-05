import React from "react";

import styled from "styled-components/native";

const Wrapper = styled.View`
    margin-top: 64px;
`;

const Img = styled.Image`
    width: 300px;
    height: 175px;
`;


export const Logo = () => {
  return (
    <Wrapper>
      <Img
        source={require("../img/anemoi-logo.png")}
      />
    </Wrapper>
  );
};
