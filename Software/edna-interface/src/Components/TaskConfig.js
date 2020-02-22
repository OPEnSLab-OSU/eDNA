import { h, Fragment } from "preact";
import { useEffect } from "preact/hooks";
import { Formik, Form, useFormik, useField, useFormikContext } from "formik";
import { css } from "@emotion/core";
import styled from "@emotion/styled";

import { Headline } from "./Headline";
import { BasicTextField, BasicTextFieldComponent, BasicTextFieldStyles } from "./TextField";
import { BasicTextArea } from "./TextArea";

import { Form as MUIForm, Button, Input, Panel } from "muicss/react";

import { useFormChange } from "../hooks";

// Need to use className instead of class because styled-component css depends on the className value
const BasicTimeFieldStyles = css`
	.textfield {
		display: grid;
		grid-auto-flow: column;
		gap: 16px;
	}
`;


const TaskScheduleTimeFields = (props) => {
	const { getFieldProps } = useFormikContext();
	return (
		<BasicTextFieldComponent css={[BasicTextFieldStyles, BasicTimeFieldStyles]} {...props}>
			{ (rest) => (
				<Fragment>
					{["hour", "minute", "second"].map(t => 
						<input key={t} name={t} type="number" placeholder={t + "s"} required {...getFieldProps(t)}/>
					)}
				</Fragment>
			)
			}
		</BasicTextFieldComponent>
	);
};

function FormikOnChange({ onChange }) {
	const { values } = useFormikContext();
	useEffect(() => {
		onChange(values);
	}, [values]);
	return null;
}


const TaskConfigFormStyles = css`
	display: grid;
	padding: 0;
	font-size: 13px;
	position: relative;
    overflow-y: scroll;
	width: 400px;
	gap: 24px;
	height: 600px;
`;

function TaskConfig(props) {
	return (
		<Formik initialValues={{
			name: "",
			scheduleDate: null,
			scheduleTime: null,
			valves: "",
			hour: 0,
			minute: 0,
			second: 0,
			notes: ""
		}} onSubmit={(values) => alert(JSON.stringify(values))}>
			{ (props) => 
				<Form className="mui-form card" css={TaskConfigFormStyles} >
					<FormikOnChange onChange={(values) => console.log(values)}/>
					<Headline>Task Settings</Headline>
					<BasicTextField 
						name="name" 
						title="Task Name"
						subtitle="Name used to identify the task"
						type="text" required/>

					<BasicTextField
						name="scheduleDate"
						title="Schedule Date"
						subtitle="Specific date when to run this task (YYYY-MM-DD)"
						type="date"/>

					<BasicTextField
						name="scheduleTime"
						title="Schedule Time"
						subtitle="Specific time when to run this task (HH:MM)"
						type="time"/>

					<BasicTextField
						name="valves"
						title="Valves"
						subtitle="Valves assigned to this task"
						type="text" placeholder="e.g. 1,2,3,4,5"/>

					<TaskScheduleTimeFields 
						title="Time Between"
						subtitle="Controls how long until the next sample"/>
			
					<BasicTextArea 
						name="notes"
						title="Notes"
						subtitle="Additional information associated with this task up to 250 characters" 
						type="text" placeholder="Describe the task (optional)"/>
					
					{/* <BasicTextField type="submit" /> */}
					<Button css={css`width: 100px; justify-self: right; margin-right: 24px; background: palevioletred; color: white;`} type="submit">Submit</Button>
					<div></div>
				</Form>
			}
		</Formik>
	);
}

export { TaskConfig };