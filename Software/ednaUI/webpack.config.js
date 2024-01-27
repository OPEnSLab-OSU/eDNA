const path = require("path");
const zlib = require("zlib");
const webpack = require("webpack"); //to access built-in plugins
const { CleanWebpackPlugin } = require("clean-webpack-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const HtmlWebpackHarddiskPlugin = require("html-webpack-harddisk-plugin");
const CompressionPlugin = require("compression-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const CssMinimizerPlugin = require("css-minimizer-webpack-plugin");
// Required in order to inline CSS and JS files into HTML
const InlineChunkHtmlPlugin = require("react-dev-utils/InlineChunkHtmlPlugin");
const HTMLInlineCSSWebpackPlugin = require("html-inline-css-webpack-plugin").default;

module.exports = (_env, argv) => {
    process.env.NODE_ENV = argv.mode;
    const isProduction = process.env.NODE_ENV == "production";
    return {
        mode: isProduction ? "production" : "development",
        context: __dirname,
        entry: "./app/index",
        output: {
            filename: "bundle.js",
            path: path.resolve(__dirname, "dist"),
            publicPath: "/",
        },
        devServer: {
            contentBase: path.resolve(__dirname, "dist"),
            compress: true,
            historyApiFallback: true,
        },
        module: {
            rules: [
                {
                    test: /\.(ts|tsx|js|jsx)$/,
                    use: [{ loader: "babel-loader" }],
                    // We exclude node_modules here assuming that all libraries already pre-compiled
                    exclude: /node_modules/,
                },
                {
                    test: /\.css$/i,
                    use: [
                        { loader: isProduction ? MiniCssExtractPlugin.loader : "style-loader" },
                        {
                            loader: "css-loader",
                            options: {
                                modules: true,
                                importLoaders: 1,
                            },
                        },
                        { loader: "postcss-loader" },
                    ],
                },
                {
                    test: /\.svg$/,
                    use: ["@svgr/webpack"],
                },
                {
                    test: /\.(png|jpg|jpeg|gif)$/i,
                    type: "asset/resource",
                },
            ],
        },
        optimization: {
            minimizer: [
                // For webpack@5 you can use the `...` syntax to extend existing minimizers (i.e. `terser-webpack-plugin`)
                `...`,
                new CssMinimizerPlugin(),
            ],
        },
        plugins: [
            new webpack.DefinePlugin({}),
            new webpack.ProgressPlugin(),
            new CleanWebpackPlugin({ cleanStaleWebpackAssets: true }),

            // Automatically generate index HTML file based on template
            new HtmlWebpackPlugin({
                inject: "body",
                title: `EDNA Dashboard ${isProduction ? "" : "(dev)"}`,
                template: path.resolve(__dirname, "app/template.ejs"),
                // alwaysWriteToDisk: true, // enable this option to write to disk regardless of the mode we are in.
            }),

            // This plugin do nothing without the `alwaysWriteToDisk: true` property above
            new HtmlWebpackHarddiskPlugin(),

            // Extract CSS imports to separate files
            // Next, inline them to the generated HTML file
            isProduction && new MiniCssExtractPlugin(),
            isProduction && new HTMLInlineCSSWebpackPlugin(),

            // Inline JS file to the generated HTML file
            isProduction && new InlineChunkHtmlPlugin(HtmlWebpackPlugin, [/\.js$/]),

            // gzip compression
            isProduction &&
                new CompressionPlugin({
                    filename: "[path][name].gz",
                    include: /\.(html)$/,
                }),

            // Brotli copression
            isProduction &&
                new CompressionPlugin({
                    filename: "[path][name].br",
                    include: /\.(html)$/,
                    algorithm: "brotliCompress",
                    compressionOptions: {
                        params: {
                            [zlib.constants.BROTLI_PARAM_QUALITY]:
                                zlib.constants.BROTLI_MAX_QUALITY,
                        },
                    },
                }),
            {
                // This function provides env information for client code
                apply: function () {
                    const build = require("./scripts/build");
                    build.writeBuildMetaData("./app/build.json");
                },
            },
        ].filter(Boolean),
        resolve: {
            extensions: [".tsx", ".ts", ".js", ".jsx"],
            alias: {
                react: "preact/compat",
                // Remove if not needed to reduce bundle size
                // "react-dom/test-utils": "preact/test-utils",

                // Must be below test-utils
                "react-dom": "preact/compat",
                app: path.resolve(__dirname, "app"),
                assets: path.resolve(__dirname, "assets"),
                components: path.resolve(__dirname, "components"),
                pages: path.resolve(__dirname, "pages"),
                styles: path.resolve(__dirname, "styles"),
                hooks: path.resolve(__dirname, "hooks"),
                lib: path.resolve(__dirname, "lib"),
                "root@redux": path.resolve(__dirname, "redux"),
            },
        },
    };
};
