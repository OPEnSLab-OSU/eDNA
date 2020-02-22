import { h } from "preact";
import { css } from "@emotion/core";
import { BasicTextFieldComponent, BasicTextFieldStyles } from "../TextField";

const BasicTextAreaStyles = css`
	textarea {
		font-size: 1.4rem;
        min-height: 100px;
        resize: vertical;
    }
`; 

const BasicTextArea = (props) => (
	<BasicTextFieldComponent css={[BasicTextFieldStyles, BasicTextAreaStyles]} {...props}>
		{(rest) => <textarea className="mui--text-dark-secondary" {...rest}/>}
	</BasicTextFieldComponent>
);

export { BasicTextArea };