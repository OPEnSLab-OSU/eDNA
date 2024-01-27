import tw, { theme } from "twin.macro";

import { useSelector } from "react-redux";

import { capitalize } from "lodash";

import { Card } from "components/modules/Card";

const states = [
    "idle",
    "flush 1",
    "offshoot clean 1",
    "flush 2",
    "sample",
    "preserve flush",
    "preserve",
    "flush 3",
    "offshoot clean 2",
    "air flush",
    "stop",
];

type StateRowProps = {
    name: string;
    isActive?: boolean;
    timeEntered?: number;
};

export const StateRow = ({ name, isActive }: StateRowProps) => {
    return (
        <div
            tw="grid grid-flow-col bg-white text-sm py-4 rounded-md shadow"
            css={[
                { gridTemplateColumns: "20% 55% 25%" },
                isActive ? tw`text-primary` : tw`text-secondary`,
            ]}>
            <div tw="grid place-items-center">
                <div tw="grid place-items-center border-2 rounded border-teal-500 w-4 h-4">
                    <div
                        tw="w-2 h-2 rounded"
                        css={{ backgroundColor: isActive ? theme`colors.teal.500` : "" }}
                    />
                </div>
            </div>
            <div>{capitalize(name)}</div>
            <div></div>
        </div>
    );
};
export const StateTable = () => {
    const status = useSelector(state => state.status);
    const currentState = status.currentState
        ?.toLowerCase()
        .replace(/_state|_/g, " ")
        .trim();
    console.log("Current State", currentState);
    return (
        <Card title="States" tw="p-0">
            <div tw="grid gap-2">
                <div
                    tw="grid grid-flow-col bg-blueGray-100 text-sm text-primary py-4 rounded-md shadow"
                    css={{ gridTemplateColumns: "20% 55% 25%" }}>
                    <div tw="justify-self-center">Active</div>
                    <div>Name</div>
                    <div>Time entered</div>
                </div>
                {states.map((s, i) => (
                    <StateRow key={s} name={s} isActive={states[i] === currentState} />
                ))}
            </div>
        </Card>
    );
};
