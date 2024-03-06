"use client"

import React, {useEffect, useState} from "react";
import useWebSocket from "react-use-websocket";
import {takeLast} from "ramda";
import {ResponsiveLine} from "@nivo/line";

export const SensorMillivoltsChart = () => {
  const [cellVoltageData, setCellVoltageData] = useState<{ x: Date; y: number }[]>([]);

  const {lastMessage} = useWebSocket("ws://192.168.1.154/ws");

  useEffect(() => {
    if (lastMessage !== null) {
      const {cellVoltage} = JSON.parse(lastMessage.data);

      const date = new Date();

      setCellVoltageData((prev) =>
        takeLast(
          60,
          prev.concat({
            x: date,
            y: parseFloat(cellVoltage),
          })
        )
      );
    }
  }, [lastMessage]);

  return (
    <div>
      <h2
        className="mt-10 scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
        Sensor millivolts
      </h2>
      <div className="h-[400px]">
        <ResponsiveLine
          margin={{top: 30, right: 50, bottom: 60, left: 50}}
          data={[
            {
              id: "cell_voltage",
              data: cellVoltageData,
            },
          ]}
          colors={["hsl(57, 70%, 50%)"]}
          enableArea
          xScale={{type: "time", format: "native"}}
          yScale={{type: "linear", max: 100}}
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