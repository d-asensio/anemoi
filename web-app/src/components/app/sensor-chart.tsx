"use client";

import React, { useEffect, useState } from "react";
import useWebSocket from "react-use-websocket";
import { last, takeLast } from "ramda";
import { ResponsiveLine } from "@nivo/line";
import { Card, CardContent } from "@/components/ui/card";

interface SensorChartProps {
  title: string;
  dataKey: string;
  maxScaleY: number;
  color: string;
  unit: string
}

export const SensorChart = ({
  title,
  dataKey,
  maxScaleY,
  color,
  unit
}: SensorChartProps) => {
  const [data, setData] = useState<{ x: Date; y: number }[]>([]);

  const { lastMessage } = useWebSocket("ws://192.168.1.134/ws");

  useEffect(() => {
    if (lastMessage !== null) {
      const parsedMessage = JSON.parse(lastMessage.data);

      const date = new Date();

      setData((prev) =>
        takeLast(
          60,
          prev.concat({
            x: date,
            y: parseFloat(parsedMessage[dataKey]),
          })
        )
      );
    }
  }, [dataKey, lastMessage]);

  return (
    <div className="space-y-4">
      <h2 className="mt-10 scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
        {title}
      </h2>
      <div className="flex h-[400px] gap-6">
        <ResponsiveLine
          margin={{ top: 5, right: 0, bottom: 5, left: 40 }}
          data={[
            {
              id: dataKey,
              data,
            },
          ]}
          colors={[color]}
          enableArea
          xScale={{ type: "time", format: "native" }}
          yScale={{ type: "linear", max: maxScaleY }}
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
            axis: { ticks: { text: { fontSize: 14 } } },
            grid: { line: { stroke: "#ddd", strokeDasharray: "1 2" } },
          }}
        />
        <Card>
          <CardContent className="flex items-center justify-center h-[400px] w-[500px]">
            <div>
              <span className="text-9xl font-extrabold mr-4">
                {(last(data) ?? { y: ' - ' }).y}
              </span>
              <span className="text-4xl">{unit}</span>
            </div>
          </CardContent>
        </Card>
      </div>
    </div>
  );
};
