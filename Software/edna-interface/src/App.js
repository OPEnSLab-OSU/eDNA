
import { h, Fragment } from "preact";
import { useContext } from "preact/hooks";

import { Global, css } from "@emotion/core";

import { TaskConfig } from "./Components";

import "./index.scss";
export default function App() {
	return (
		<Fragment>
			<TaskConfig />
		</Fragment>
	);
}