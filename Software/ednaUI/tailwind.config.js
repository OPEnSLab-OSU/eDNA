const plugin = require("tailwindcss/plugin");
const palette = require("tailwindcss/colors");
const colors = {
    ...palette,
    accent: palette.teal[500],
    "accent-red": palette.red[500],
    "accent-yellow": palette.yellow[500],
    primary: palette.trueGray[900],
    secondary: palette.coolGray[400],
    background: palette.trueGray[100],
    foreground: palette.white,
};

const typographyPlugin = plugin(function ({ addComponents }) {
    const types = {
        ".text-display": {
            fontSize: "2.25rem",
            fontWeight: "bold",
            lineHeight: "2.5rem",
            letterSpacing: "0",
            textTransform: "none",
        },
        ".text-title": {
            fontSize: "1.5rem",
            fontWeight: "bold",
            lineHeight: "2rem",
            letterSpacing: "0",
            textTransform: "none",
        },
        ".text-subtitle": {
            fontSize: "0.875rem",
            fontWeight: "normal",
            lineHeight: "1.125rem",
            letterSpacing: "0",
            textTransform: "none",
        },
        ".text-overline": {
            fontSize: "0.75rem",
            fontWeight: "normal",
            lineHeight: "1rem",
            letterSpacing: "0.025rem",
            textTransform: "uppercase",
        },
        ".text-body": {
            fontSize: "0.875rem",
            fontWeight: "normal",
            lineHeight: "1.125rem",
            letterSpacing: "0px",
            textTransform: "none",
        },
    };

    addComponents(types);
});

module.exports = {
    theme: {
        extend: {
            colors: colors,
            textColor: {
                primary: colors.coolGray[700],
            },
        },
        fontFamily: {
            sans: ["Arial"],
        },
        screens: {
            base: "0px",
            md: "768px",
            // => @media (min-width: 768px) { ... }

            lg: "1024px",
            // => @media (min-width: 1024px) { ... }

            xl: "1280px",
            // => @media (min-width: 1280px) { ... }
        },
    },
    plugins: [
        require("tailwindcss-debug-screens"),
        require("@tailwindcss/custom-forms"),
        typographyPlugin,
    ],
};
