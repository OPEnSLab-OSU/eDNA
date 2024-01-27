import { useEffect, useRef } from "react";
import tw, { styled } from "twin.macro";
import { Check, X } from "phosphor-react";

import { Button } from "components/units/Button";

export type NewTaskInputProps = {
    onSubmit: (name: string) => void;
    onCancel: () => void;
    hide: boolean;
};

const Background = styled.div<{ hide: boolean }>`
    ${tw`grid fixed place-items-center top-0 left-0 h-screen w-screen z-50 p-10 visible opacity-100`};

    transition: visibility 0s, opacity 0.1s ease-in-out;
    background-color: rgba(0, 0, 0, 0.85);
    ${props => props.hide && tw`invisible opacity-0`};
`;

export const NewTaskInput = ({ onSubmit, onCancel, hide }: NewTaskInputProps) => {
    const inputRef = useRef<HTMLInputElement>(null);
    useEffect(() => {
        const input = inputRef.current;
        if (!input) return;

        const handler = (e: KeyboardEvent) => {
            if (e.key === "Enter") {
                onSubmit(input.value);
            }
        };

        input.focus();
        input.addEventListener("keydown", handler);
        return () => {
            input.removeEventListener("keydown", handler);
        };
    }, [onSubmit]);
    return (
        <Background hide={hide}>
            <div
                tw="bg-trueGray-100 grid place-items-center h-full shadow-xl rounded"
                css={{ width: "calc(100vw - 100px)", gridTemplateRows: "1fr min-content" }}>
                <label tw="grid gap-4 max-w-screen-md w-full">
                    <span tw="text-title font-normal text-primary whitespace-nowrap self-center">
                        Enter new task name
                    </span>
                    <input
                        tw="form-input text-xl bg-transparent border border-secondary rounded flex-1"
                        ref={inputRef}
                        type="text"
                    />

                    <div tw="grid grid-flow-col gap-4 justify-start items-center">
                        <Button
                            tw="uppercase p-3 bg-teal-500 text-white flex-shrink justify-self-start"
                            text="OK"
                            icon={<Check size={20} />}
                            iconRight
                            onClick={() => {
                                inputRef.current && onSubmit(inputRef.current.value);
                            }}
                        />
                        <span>
                            or press <span tw="text-sm font-bold">Enter ⏎</span>
                        </span>
                    </div>
                </label>
                <Button
                    tw="uppercase bg-transparent shadow-none mb-10 hover:(shadow-none text-accent)"
                    text="close"
                    icon={<X />}
                    onClick={onCancel}
                />
            </div>
        </Background>
    );
};
