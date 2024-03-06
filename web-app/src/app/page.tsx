"use client";

import { MetadataTable } from "@/components/app/metadata-table";
import { SensorChart } from "@/components/app/sensor-chart";
import React from "react";

export default function Home() {
  return (
    <main className="container p-6">
      <h1 className="scroll-m-20 text-4xl font-extrabold tracking-tight lg:text-5xl mb-4">
        Anemoi Analyzer
      </h1>
      <div className="space-y-6">
        <MetadataTable />
        <SensorChart
          title="Cell millivolts"
          dataKey="cellVoltage"
          maxScaleY={60}
          color="hsl(57, 70%, 50%)"
          unit="mV"
        />
        <SensorChart
          title="O2 Partial Pressure"
          dataKey="ppO2"
          maxScaleY={1}
          color="hsl(142, 70%, 50%)"
          unit="ppO2"
        />
        <SensorChart
          title="Ambient Temperature"
          dataKey="temperature"
          maxScaleY={40}
          color="hsl(208, 70%, 50%)"
          unit="℃"
        />
        <SensorChart
          title="Altitude"
          dataKey="altitude"
          maxScaleY={100}
          color="hsl(2, 70%, 50%)"
          unit="m"
        />
      </div>
    </main>
  );
}
