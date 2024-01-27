const fs = require("fs");

const execa = require("execa");

const PACKAGE = require("../package.json");

// export type Meta = {
//     env: "development" | "production"
//     name: string;
//     version: string;
//     date: number;
//     hash: string;
//     commitNumber: number;
//     dirty: boolean;
// };

function getBuildMetaData() {
    const gitHash = execa.sync("git", ["rev-parse", "--short", "HEAD"]).stdout;
    const gitNumCommits = Number(execa.sync("git", ["rev-list", "HEAD", "--count"]).stdout);
    const gitDirty = execa.sync("git", ["status", "-s", "-uall"]).stdout.length > 0;

    return {
        env: process.env.NODE_ENV,
        name: PACKAGE.name,
        version: PACKAGE.version,
        date: Date.now(),
        hash: gitHash,
        commitNumber: gitNumCommits,
        dirty: gitDirty,
    };
}

function writeBuildMetaData(path) {
    fs.writeFileSync(path, JSON.stringify(getBuildMetaData(), null, 4) + "\n");
}

module.exports = { getBuildMetaData, writeBuildMetaData };
