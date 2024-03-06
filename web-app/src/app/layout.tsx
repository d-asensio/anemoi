import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Anemoi Analyzer",
  description: "A modern gas analyzer for technical divers",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body className="font-sans">{children}</body>
    </html>
  );
}
