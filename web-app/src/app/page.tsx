"use client"

import {MetadataTable} from "@/components/app/metadata-table";
import {SensorMillivoltsChart} from "@/components/app/sensor-millivolts-chart";
import {O2PartialPressureChart} from "@/components/app/o2-partial-pressure-chart";
import React from "react";

export default function Home() {
  return (
    <main className="container p-6">
      <h1 className="scroll-m-20 text-4xl font-extrabold tracking-tight lg:text-5xl mb-4">
        Anemoi Analyzer
      </h1>
      <div>
        <MetadataTable/>
        <SensorMillivoltsChart/>
        <O2PartialPressureChart/>
      </div>
    </main>
  );
}
