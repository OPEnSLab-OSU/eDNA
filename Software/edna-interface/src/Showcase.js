import { h } from "preact";
import styled from "@emotion/styled"; 
import { TaskConfig } from "./Components";

const ShowcaseBase = styled.div`
	display: grid;
	grid: ${props => props.grid ?? "min-content / min-content"};
	justify-content: center;
	align-content: center;
	height: 100vh;
`;

function Showcase() {
	return (
		<ShowcaseBase>
			<TaskConfig></TaskConfig>
		</ShowcaseBase>
	);
}

export default Showcase; 