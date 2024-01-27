module.exports = {
    root: true,
    extends: [
        // Default recommended rules
        "eslint:recommended",

        // Recommended rules from @typescript-eslint/eslint-plugin
        "plugin:@typescript-eslint/recommended",

        // Recommended rules from eslint-plugin-react
        "plugin:react/recommended",

        // Recommneded React hooks rules
        "plugin:react-hooks/recommended",

        // Recommended accessibility rules
        // See https://developer.mozilla.org/en-US/docs/Web/Accessibility
        // Also see https://github.com/jsx-eslint/eslint-plugin-jsx-a11y
        "plugin:jsx-a11y/recommended",

        // Turn off Typescript rules that would conflict with prettier
        // See https://github.com/prettier/eslint-plugin-prettier
        "prettier/@typescript-eslint",

        // Recommneded rules from eslint-plugin-prettier
        // Modify the final formatting rules based on prettier
        "plugin:prettier/recommended",

        // eslint-plugin-import presets
        // See https://github.com/benmosher/eslint-plugin-import/tree/master/config
        // Also see https://github.com/benmosher/eslint-plugin-import/tree/master/docs/rules
        "plugin:import/errors",
        "plugin:import/warnings",
        "plugin:import/typescript",
    ],

    // Allows eslint to parse Typescript code.
    // Use @typescript-eslint/parser in conjunction with @typescript-eslint/eslint-plugin
    // See https://www.npmjs.com/package/@typescript-eslint/eslint-plugin
    parser: "@typescript-eslint/parser",

    // Ignore generated files
    ignorePatterns: [
        ".next",
        "node_modules",
        "dist",
        "public",
        "build",
        "out",
        "!.lintstagedrc.js",
        "!.storybook",
    ],

    // Enable global variables for browser, node, and ES6
    env: {
        es6: true,
        browser: true,
        node: true,
    },

    parserOptions: {
        // Allows for the use of imports
        sourceType: "module",

        // Enable modern ECMA script features
        ecmaVersion: 2020,
        ecmaFeatures: {
            jsx: true, // Parse JSX
        },
    },
    plugins: ["react", "@typescript-eslint", "prettier"],
    settings: {
        react: {
            // eslint-plugin-preact interprets this as "h.createElement",
            // however we only care about marking h() as being a used variable.
            pragma: "h",
            // We use "react 16.0" to avoid pushing folks to UNSAFE_ methods.
            version: "16.0",
        },
        "import/resolver": {
            webpack: {
                config: "./webpack.config.js",
            },
            alias: [
                ["react", "preact/compat"],
                ["react-dom", "preact/compat"],
            ],
        },
    },
    rules: {
        // Editor
        "linebreak-style": ["error", "unix"],

        // Code
        "class-methods-use-this": "off",
        "comma-dangle": "off",
        "function-paren-newline": "off",
        "global-require": "off",

        // This rule is not compatible with Next.js's <Link /> components
        "jsx-a11y/anchor-is-valid": "off",
        "no-inner-declarations": "off",

        // Turn off default no-unused-vars rule and use Typescript version instead
        "no-unused-vars": "off",
        "@typescript-eslint/no-unused-vars": [
            "warn",
            { argsIgnorePattern: "^_", varsIgnorePattern: "^_|tw" },
        ],

        "react/prop-types": "off",
        "react/react-in-jsx-scope": "off",
        "react/display-name": "off",

        // See https://github.com/typescript-eslint/typescript-eslint/blob/master/packages/eslint-plugin/docs/rules/ban-types.md
        // Use Record<string, unknown> instead of object
        "@typescript-eslint/ban-types": "error",
        "@typescript-eslint/explicit-module-boundary-types": "off",
        "@typescript-eslint/explicit-function-return-type": "off",
        "@typescript-eslint/no-empty-interface": "off", // We need this to augment TS interface
        "@typescript-eslint/no-var-requires": "off",

        // Import ordering
        "import/order": [
            "error",
            {
                "newlines-between": "always-and-inside-groups",
                groups: ["builtin", "external", "internal", "parent", "sibling", "index"],
            },
        ],
    },
};
