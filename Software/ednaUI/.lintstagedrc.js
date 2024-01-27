module.exports = {
    "*.{ts,tsx,js,jsx}": names => {
        return [`eslint ${names.join(" ")}`, "tsc --pretty --noEmit"];
    },
};
