"use client"

import useWebSocket, {ReadyState} from "react-use-websocket";
import {Table, TableBody, TableCell, TableRow} from "@/components/ui/table";
import {Badge} from "@/components/ui/badge";
import React from "react";

export const MetadataTable = () => {
  const {readyState} = useWebSocket("ws://192.168.1.154/ws");

  const connectionStatus = {
    [ReadyState.CONNECTING]: "Connecting",
    [ReadyState.OPEN]: "Open",
    [ReadyState.CLOSING]: "Closing",
    [ReadyState.CLOSED]: "Closed",
    [ReadyState.UNINSTANTIATED]: "Uninstantiated",
  }[readyState];

  return (
    <Table className="max-w-[300px]">
      <TableBody>
        <TableRow>
          <TableCell className="font-medium">Connection</TableCell>
          <TableCell className="text-right">
            <Badge>{connectionStatus}</Badge>
          </TableCell>
        </TableRow>
      </TableBody>
    </Table>
  );
};