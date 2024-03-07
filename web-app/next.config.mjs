import createWithBundleAnalyzer from "@next/bundle-analyzer";

const withBundleAnalyzer = createWithBundleAnalyzer({
  enabled: process.env.ANALYZE === "true",
});

/** @type {import('next').NextConfig} */
const nextConfig = {
  output: "export",
};

export default withBundleAnalyzer(nextConfig);
