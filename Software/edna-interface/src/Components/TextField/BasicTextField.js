import { h } from "preact";
import { Input } from "muicss/react";
import { css } from "@emotion/core";
import styled from "@emotion/styled";
import { useField } from "formik";

const Subtitle = (props) => <p className={"mui--text-dark-secondary subtitle"} {...props}/>;
export const BasicTextFieldComponent = (props) => {
	const {
		title, 
		subtitle, 
		className, 
		required = true, 
		...componentProps 
	} = props;

	// Maybe a duplicate from preact/compat layer
	// We have both className and class props so remove class
	delete componentProps.class; 

	return (
		<div className={`${className} section`}>
			<div className={"mui-textfield textfield"} css={css`margin-bottom: 0;`}>
				{props.children ? props.children(componentProps) : <input required={required} {...componentProps}/>}
				{<label className="title">{title}</label>}
			</div>
			{subtitle && <Subtitle>{subtitle}</Subtitle>}
		</div>
	); 
};

export const BasicTextFieldStyles = css`
	display: grid;
	padding: 0 24px;
	grid-auto-flow: row;

	input {
		font-size: 1.4rem;
	} 
	
	.title, .subtitle {
		font-size: 1.2rem;
		margin: 0;
	}

	.subtitle {
		margin-top: 4px;
	}
`;

export const BasicTextField = (props) => {
	const [fields, metas, helpers] = useField(props.name);
	return <BasicTextFieldComponent css={BasicTextFieldStyles} {...fields} {...props}/>;
};





