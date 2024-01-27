import { render } from "preact";
import { GlobalStyles } from "twin.macro";
import { HashRouter as Router } from "react-router-dom";

import { Application } from "./app";

if (typeof window !== "undefined") {
    render(
        <Router>
            <GlobalStyles />
            <Application />
        </Router>,
        document.getElementById("root") as HTMLElement
    );
}
