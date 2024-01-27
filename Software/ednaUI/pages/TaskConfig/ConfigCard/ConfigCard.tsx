import tw from "twin.macro";
import { useFormContext } from "react-hook-form";
import { forwardRef } from "react";

import { InputField } from "components/units/InputField";

import { FormValues, FieldProps } from "../data";

export type ConfigCardProps = {
    title: string;
    className?: string;
    highlight: boolean;
    fields: FieldProps[];
};

export const ConfigCard = forwardRef<HTMLDivElement, ConfigCardProps>(
    ({ title, className, highlight, fields }, ref) => {
        const { register, errors } = useFormContext<FormValues>();
        return (
            <div
                id={title.toLowerCase()}
                tw="p-8 grid gap-8 bg-white rounded-xl"
                css={highlight ? tw`shadow-lg` : {}}
                className={className}
                ref={ref}>
                <h3 tw="text-title text-primary">{title}</h3>
                {fields.map(props => (
                    <InputField
                        key={props.name}
                        ref={register({
                            valueAsNumber: props.type === "number",
                        })}
                        error={errors[props.name]?.message}
                        {...props}
                    />
                ))}
            </div>
        );
    }
);
