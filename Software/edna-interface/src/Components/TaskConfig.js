import { h } from "preact";
import { useEffect } from "preact/hooks";
import { Formik, useFormik, useFormikContext, Form } from "formik";
import { css } from "@emotion/core";
import styled from "@emotion/styled";

import { Headline } from "./Headline";
import { BasicTextField, BasicTextFieldComponent, BasicTextFieldStyles } from "./TextField";
import { BasicTextArea } from "./TextArea";

import { Form as MUIForm, Button, Input, Panel } from "muicss/react";

import { useFormChange } from "../hooks";

// Need to use className instead of class because styled-component css depends on the className value
const BasicTimeFieldStyles = css`
	form {
        display: flex;
        overflow: hidden;
        justify-content: space-between;

		div {
			padding: 0;
		}

		label {
			display: none;
		}

		input {
			width: 100px;
		}
    }
`;

const TaskConfigFormStyles = css`
	display: grid;
	padding: 0;
    font-size: 13px;
    overflow: hidden;
	width: 400px;
	gap: 24px;
`;

const TaskScheduleTimeFields = (props) => (
	<BasicTextFieldComponent css={[BasicTextFieldStyles, BasicTimeFieldStyles]} {...props}>
		{ (props) =>
			<form >
				{["hour", "minute", "second"].map(t => 
					<BasicTextField 
						key={t}
						name={t}
						type="number" 
						placeholder={t + "s"} {...props}/>
				)}
			</form>
		}
	</BasicTextFieldComponent>
);

function TaskConfig(props) {
	const formik = useFormik({
		initialValues: {},
		onChange: (values) => console.log(values)
	});

	return (
		<form className="mui-form card" css={TaskConfigFormStyles} onSubmit={formik.onSubmit}>
			<Headline>Task Settings</Headline>

			<BasicTextField 
				name="name" 
				title="Task Name"
				subtitle="Name used to identify the task"
				type="text" required onChange={formik.handleChange}/>

			<BasicTextField
				name="scheduleDate"
				title="Schedule Date"
				subtitle="Specific date when to run this task (YYYY-MM-DD)"
				type="date"/>

			<BasicTextField
				name="scheduleTime"
				title="Schedule Time"
				subtitle="Specific time when to run this task (HH:MM)"
				type="time" placeholder=""/>

			<BasicTextField
				name="valves"
				title="Valves"
				subtitle="Valves assigned to this task"
				type="text" placeholder="e.g. 1,2,3,4,5"/>

			<TaskScheduleTimeFields 
				name="timeBetween" 
				title="Time Between"
				subtitle="Controls how long until the next sample"/>

			<BasicTextArea 
				name="notes"
				title="Notes"
				subtitle="Additional information associated with this task up to 250 characters" 
				type="text" placeholder="Describe the task (optional)"/>
			<div></div>
		</form>
	);
}

export { TaskConfig };