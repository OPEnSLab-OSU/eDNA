
import { h } from "preact";

import { css } from "@emotion/core";

import { AppContext, AppContextProvider } from "./AppContext.js";
import { RightBar } from "./Components";

import "./index.scss";

const AppStyles = css`
	display: grid;
	grid-template-columns: 240px 1fr min-content;
	grid-template-rows: 80px 1fr;
	grid-template-areas:
    "sidebar dropbar rightbar"
	"sidebar main    rightbar";

	padding-right: 24px;

	max-height: 100vh;
	overflow: hidden;
`;
export default function App() {
	const goodies = { groups: ["Task 1", "Task 2", "Task 3"] };
	return (
		<AppContextProvider value={goodies}>
			<div css={AppStyles}>
				<RightBar />
			</div>
		</AppContextProvider>
	);
}