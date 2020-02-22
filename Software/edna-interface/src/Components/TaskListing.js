
import { h } from "preact";
import { useContext } from "preact/hooks";
import { css } from "@emotion/core";
import { Headline } from "./Headline"; 

import { AppContext } from "../AppContext.js";

const GroupListingStyles = css`
	display: grid;
	grid-auto-rows: min-content;
	padding: 0;
	font-size: 13px;
	gap: 24px;
	/* height: 650px; */
	grid-area: group-listing;
	background: white;
`;

function TaskListing(props) {
	const { groups } = useContext(AppContext);
	return (
		<div className="mui-form card" css={GroupListingStyles}>
			<Headline>Group List</Headline>
			{groups.map(g => (<div key={g} css={css`padding: 0 24px;`}>{g}</div>))}
			<div />
		</div>
	);
}


export { TaskListing };