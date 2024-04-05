import React from "react";

import {Image, View} from "react-native";

export const PulseIndicator = () => {
  console.log(require("./img/anemoi-logo.png"))
  return (
    <View>
      <Image
        style={{
          width: 300,
          height: 175
        }}
        source={require("./img/anemoi-logo.png")}
      />
    </View>
  );
};
