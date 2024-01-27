import { ComponentPropsWithoutRef, forwardRef, ReactNode } from "react";
import tw from "twin.macro";

export type InputFieldProps = ComponentPropsWithoutRef<"input"> & {
    name: string;
    label: string;
    sublabel?: string;
    helperText?: string;
    error?: string;
    mapItemList?: (items: ReactNode[]) => ReactNode[];
};

const LabelSpan = tw.span`flex justify-between items-center`;
const HelperSpan = tw.span`flex text-xs text-gray-500`;
const Input = tw.input`form-input text-sm bg-transparent border border-primary rounded`;
const ErrorMessage = tw.p`text-sm text-red-700 bg-red-200 p-2 rounded`;

export const InputField = forwardRef<HTMLInputElement, InputFieldProps>((props, ref) => {
    const { name, label, sublabel, helperText, mapItemList, error, ...inputProps } = props;

    const defaultItemList: ReactNode[] = [
        <LabelSpan key="label">
            <span tw="text-sm">{label + (props.required ? " *" : "")}</span>
            <span tw="text-xs text-secondary">{sublabel}</span>
        </LabelSpan>,
        <Input key="input" name={name} ref={ref} {...inputProps} />,
    ].filter(Boolean);

    if (helperText) {
        defaultItemList.push(<HelperSpan key="helper">{helperText}</HelperSpan>);
    }

    if (error) {
        defaultItemList.push(<ErrorMessage key="error">{error}</ErrorMessage>);
    }

    const itemList = mapItemList ? mapItemList(defaultItemList) : defaultItemList;
    return <label tw="grid gap-2">{itemList}</label>;
});

InputField.displayName = "InputField";
