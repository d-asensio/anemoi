import React from "react";

import {
  Canvas,
  Circle,
  Image,
  useClockValue,
  useComputedValue,
  useImage,
} from "@shopify/react-native-skia";
import { View } from "react-native";

export const PulseIndicator = () => {
  const clock1 = useClockValue();
  const expo = useImage(require("./img/expo.png"));

  const interval = 1250;

  const scale = useComputedValue(() => {
    return ((clock1.current % interval) / interval) * 130;
  }, [clock1]);

  const opacity = useComputedValue(() => {
    return 0.9 - (clock1.current % interval) / interval;
  }, [clock1]);

  const scale2 = useComputedValue(() => {
    return (((clock1.current + 400) % interval) / interval) * 130;
  }, [clock1]);

  const opacity2 = useComputedValue(() => {
    return 0.9 - ((clock1.current + 400) % interval) / interval;
  }, [clock1]);

  if (!expo) {
    return <View />;
  }

  return (
    <Canvas style={{ height: 300, width: 300 }}>
      <Circle cx={150} cy={150} r={50} opacity={1} color="#f9dc5c"></Circle>
      <Circle cx={150} cy={150} r={scale} opacity={opacity} color="#f9dc5c" />
      <Circle cx={150} cy={150} r={scale2} opacity={opacity2} color="#f9dc5c" />
      <Image
        image={expo}
        fit="contain"
        x={125}
        y={125}
        width={50}
        height={50}
      />
    </Canvas>
  );
};
