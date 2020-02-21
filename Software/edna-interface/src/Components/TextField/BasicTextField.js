import { h } from "preact";
import { Input } from "muicss/react";
import { css } from "@emotion/core";
import styled from "@emotion/styled";

const Subtitle = (props) => <p className={"mui--text-dark-secondary subtitle"} {...props}/>;
export const BasicTextFieldComponent = (props) => {
	const {
		title, 
		subtitle, 
		className, 
		required = true, 
		...componentProps 
	} = props;

	delete componentProps.class;
	return (
		<div className={className}>
			<div className={"mui-textfield"} css={css`margin-bottom: 0;`}>
				{props.children ? props.children(props)  : <input required {...componentProps}/>}
				<label className="title">{title}</label>
				{subtitle && <Subtitle>{subtitle}</Subtitle>}
			</div>
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

// export const BasicTextField = (props) => <BasicTextFieldComponent css={[BasicTextFieldStyles]} {...props}/>;
export const BasicTextField = (props) => <BasicTextFieldComponent css={BasicTextFieldStyles} {...props}/>;





