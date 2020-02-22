import { h, Fragment } from "preact";
import { css } from "@emotion/core";
import { TaskConfig } from "./";
import { TaskListing } from "./TaskListing";

const RightBarStyles = css`
	display: grid;
	grid-auto-flow: row;
	grid-template-rows: 1fr content;
	gap: 24px;
	grid-template-areas:
	"group-listing"
	"group-config";

	width: 360px;

	grid-area: rightbar;

	padding-top: 24px;
	padding-bottom: 24px;
`;

function RightBar() {
	return (
		<div css={RightBarStyles}>
			<TaskListing />
			<TaskConfig/>
		</div>
	);
}

export { RightBar };

