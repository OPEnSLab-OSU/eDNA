import { capitalize } from "lodash";
import { ReactElement } from "react";
import "twin.macro";

type TileProps = {
    title: string;
    value?: string | number;
    unit?: string;
    sensor?: string;
    icon?: ReactElement;
    className?: string;
};

export const SensorTile = ({
    title,
    sensor,
    value = "------",
    unit = "--",
    icon,
    className,
}: TileProps) => {
    return (
        <div
            tw="grid items-center gap-2 p-6 content-start bg-white rounded-lg shadow"
            className={className}>
            <div tw="flex justify-between text-primary">
                <span tw="text-sm font-medium">{title}</span>
                <span tw="flex-shrink  -mt-1">{icon}</span>
            </div>
            <div>
                <span tw="mr-1 text-2xl font-bold text-primary">{value}</span>
                <span tw="text-sm text-secondary">{unit}</span>
            </div>
            {sensor && (
                <div tw="text-xs text-blueGray-500 px-2 py-1 mt-2 justify-self-end bg-blueGray-100 rounded">
                    {capitalize(`${sensor} sensor`)}
                </div>
            )}
        </div>
    );
};
