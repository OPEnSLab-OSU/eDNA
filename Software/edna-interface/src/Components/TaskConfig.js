import { h, Fragment } from "preact";
import { useEffect } from "preact/hooks";
import { Formik, Form, useFormikContext } from "formik";
import { css } from "@emotion/core";

import { Headline } from "./Headline";
import { BasicTextField, BasicTextFieldComponent, BasicTextFieldStyles } from "./TextField";
import { BasicTextArea } from "./TextArea";

import { Button } from "muicss/react";

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
			{ (_) => (
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
    overflow-y: scroll;
	gap: 24px;
	/* height: 650px; */
	grid-area: group-config;
	/* padding-bottom */
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
			{ (_) => 
				<Form className="mui-form card" css={TaskConfigFormStyles} >
					<FormikOnChange onChange={(values) => console.log(values)}/>
					<Headline>Group Settings</Headline>
					<BasicTextField 
						name="name" 
						title="Group Name"
						subtitle="Name used to identify the valve group"
						type="text" required/>

					<BasicTextField
						name="scheduleDate"
						title="Schedule Date"
						subtitle="Specific date when to run this group (YYYY-MM-DD)"
						type="date"/>

					<BasicTextField
						name="scheduleTime"
						title="Schedule Time"
						subtitle="Specific time when to run this group (HH:MM)"
						type="time"/>

					<BasicTextField
						name="valves"
						title="Valves"
						subtitle="Valves assigned to this task"
						type="text" placeholder="e.g. 1,2,3,4,5"/>

					<TaskScheduleTimeFields 
						title="Time Between"
						subtitle="Controls how long until the next sample in the group"/>
			
					<BasicTextArea 
						name="notes"
						title="Notes"
						subtitle="Additional information associated with this group up to 250 characters" 
						type="text" placeholder="Describe what this group is for memo (optional)"/>
					
					<Button css={css`
						width: 100px;
						justify-self: right;
						margin-right: 24px;
						background: palevioletred;
						color: white;
					`} type="submit">
						Submit
					</Button>

					<div>&nbsp;</div>
				</Form>
			}
		</Formik>
	);
}

export { TaskConfig };