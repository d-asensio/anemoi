"use client"

import React, {useEffect, useState} from "react";
import useWebSocket from "react-use-websocket";
import {takeLast} from "ramda";
import {ResponsiveLine} from "@nivo/line";

export const O2PartialPressureChart = () => {
  const [ppO2Data, setPpO2Data] = useState<{ x: Date; y: number }[]>([]);

  const {lastMessage} = useWebSocket("ws://192.168.1.154/ws");

  useEffect(() => {
    if (lastMessage !== null) {
      const {ppO2} = JSON.parse(lastMessage.data);

      const date = new Date();

      setPpO2Data((prev) =>
        takeLast(
          60,
          prev.concat({
            x: date,
            y: parseFloat(ppO2),
          })
        )
      );
    }
  }, [lastMessage]);

  return (
    <div>
      <h2
        className="mt-10 scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
        O2 Partial Pressure
      </h2>
      <div className="h-[400px]">
        <ResponsiveLine
          margin={{top: 30, right: 50, bottom: 60, left: 50}}
          data={[
            {
              id: "ppO2",
              data: ppO2Data,
            },
          ]}
          colors={["hsl(142, 70%, 50%)"]}
          enableArea
          xScale={{type: "time", format: "native"}}
          yScale={{type: "linear", max: 1}}
          axisTop={null}
          axisBottom={null}
          axisRight={null}
          enablePoints={false}
          enableGridX={true}
          curve="monotoneX"
          animate={false}
          isInteractive={false}
          enableSlices={false}
          useMesh={true}
          theme={{
            axis: {ticks: {text: {fontSize: 14}}},
            grid: {line: {stroke: "#ddd", strokeDasharray: "1 2"}},
          }}
        />
      </div>
    </div>
  );
};