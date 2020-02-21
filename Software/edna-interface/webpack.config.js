const path = require("path");
const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const HtmlWebpackHarddiskPlugin = require("html-webpack-harddisk-plugin");
const CompressionPlugin = require("compression-webpack-plugin");
const HtmlWebpackInlineSourcePlugin = require("html-webpack-inline-source-plugin");
const { CleanWebpackPlugin } = require("clean-webpack-plugin");

const entryPath = path.resolve(__dirname, "src/index.js");
const outputPath = path.resolve(__dirname, "dist");

module.exports = {
	entry: entryPath,
	output: {
		path: outputPath,
		filename: "bundle.js",
		hotUpdateChunkFilename: "hot/hot-update.js",
		hotUpdateMainFilename: "hot/hot-update.json"
	},
	devServer: { compress: true },
	plugins: [
		new webpack.ProgressPlugin(),
		new CleanWebpackPlugin(),
		new HtmlWebpackPlugin({
			template: path.resolve(__dirname, "src/index.html"),
			filename: "index.html",
			inlineSource: ".(js|css)$",
			alwaysWriteToDisk: true
		}),
		new CompressionPlugin(),
		new HtmlWebpackInlineSourcePlugin(),
		new HtmlWebpackHarddiskPlugin(),
		new webpack.HotModuleReplacementPlugin()
	],
	resolve: {
		alias: {
			// Preact compatability layer for existing React libraries
			"react": "preact/compat",
			"react-dom": "preact/compat"
		}
	  },
	module: {
		rules: [
			{
				test: /\.jsx?$/,
				exclude: [path.resolve(__dirname, "node_modules")],
				loader: "babel-loader"
			},
			{
				test: /\.css$/,
				exclude: [path.resolve(__dirname, "node_modules")],
				loaders: ["style-loader", "css-loader"]
			},
			{
				test: /\.s[ac]ss$/,
				exclude: [path.resolve(__dirname, "node_modules")],
				loaders: ["style-loader", "css-loader", "sass-loader"]
			}
		]
	}
};
