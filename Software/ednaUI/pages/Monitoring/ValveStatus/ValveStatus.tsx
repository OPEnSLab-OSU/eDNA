import { useMemo } from "react";
import { useSelector } from "react-redux";
import tw, { styled } from "twin.macro";
import { times } from "lodash";

import { Card } from "components/modules/Card";

export const partition = <T extends unknown>(ary: T[], predicate: (elem: T) => boolean) => {
    const a: T[] = [];
    const b: T[] = [];
    ary.forEach(e => (predicate(e) ? a.push(e) : b.push(e)));
    return [a, b] as const;
};

interface Valve {
    id: number;
    status: number;
}

type ValveStatus = "unavailable" | "sampled" | "free" | "operating" | "next";
function mapStatusNumberToText(status: number): ValveStatus {
    const statusText: ValveStatus[] = ["sampled", "free", "operating"];
    return status > 0 && status < statusText.length ? statusText[status] : "unavailable";
}

function mapStatusToStyle(status: number) {
    switch (mapStatusNumberToText(status)) {
        case "sampled":
            return tw`text-accent`;
        case "free":
            return tw`text-primary`;
        case "operating":
            return tw`bg-teal-400 animate-pulse`;
        case "next":
            return tw`bg-yellow-400`;
        case "unavailable":
            return tw`text-secondary`;
    }
}

const Valve = styled.div<{ status: number }>`
    ${tw`flex items-center justify-center px-4 py-2 rounded-lg font-bold text-primary hover:bg-teal-100`}
    ${props => mapStatusToStyle(props.status)}
`;

export const ValveCollection = ({ valves }: { valves: Valve[] }) => {
    return (
        <div tw="grid h-24 grid-flow-row grid-cols-12 grid-rows-2 p-2 rounded-lg overflow-hidden shadow bg-white">
            {valves.map(v => (
                <Valve key={v.id} status={v.status}>
                    {v.id}
                </Valve>
            ))}
        </div>
    );
};

export const ValveStatus = ({ className }: { className?: string }) => {
    const status = useSelector(state => state.status);
    const valves = useMemo(() => {
        return times(24, (id: number) => {
            return status.valves && id < status.valves.length
                ? { id, status: status.valves[id] }
                : { id, status: -1 };
        });
    }, [status.valves]);

    const [top, bottom] = partition(valves, ({ id }) => id < 12);
    return (
        <Card title="Valve Status" tw="p-0" className={className}>
            <ValveCollection valves={top.concat(bottom.reverse())} />

            <div tw="grid grid-flow-col gap-8 auto-cols-max">
                <div tw="flex items-center mt-8">
                    <div tw="w-4 h-4 mr-2 bg-teal-400 rounded-md"></div>
                    <div tw="text-sm text-secondary">Current Valve</div>
                </div>

                <div tw="flex items-center mt-8">
                    <div tw="w-4 h-4 mr-2 bg-yellow-400 rounded-md"></div>
                    <div tw="text-sm text-secondary">Next</div>
                </div>

                <div tw="flex items-center mt-8">
                    <div tw="w-4 h-4 mr-2 rounded-md bg-trueGray-900"></div>
                    <div tw="text-sm text-secondary">Free</div>
                </div>

                <div tw="flex items-center mt-8">
                    <div tw="w-4 h-4 mr-2 rounded-md bg-trueGray-400"></div>
                    <div tw="text-sm text-secondary">Disabled</div>
                </div>
            </div>
        </Card>
    );
};
