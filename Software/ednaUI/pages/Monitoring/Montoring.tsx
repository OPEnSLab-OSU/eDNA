import { css } from "twin.macro";
import { useSelector } from "react-redux";

import { mapValues } from "lodash";

import { Anchor, ArrowsIn, Cloud, Drop, DropHalf, Thermometer } from "phosphor-react";

import { pick } from "lib";

import { TileCollection } from "components/modules/TileCollection";

import { useScreen, useStatusUpdate } from "hooks";

import { StateTable } from "./StateTable";
import { SensorTile } from "./SensorTile";
import { ValveStatus } from "./ValveStatus";

const Tiling = ({ columns }: { columns: number }) => {
    const status = useSelector(state => state.status);
    const { pressure, waterFlow, temperature, barometric, waterVolume, waterDepth } = mapValues(
        pick(
            status,
            "pressure",
            "waterFlow",
            "temperature",
            "barometric",
            "waterVolume",
            "waterDepth",
            "sampleVolume"
        ),
        value => value?.toFixed(2) ?? "?"
    );

    return (
        <TileCollection columns={columns} title="Sensor Data" tw="p-0">
            <SensorTile
                title="Presssure"
                value={pressure}
                unit="psi"
                icon={<ArrowsIn size={24} />}
            />
            <SensorTile
                title="Temperature"
                value={temperature}
                sensor="pressure"
                unit="°C"
                icon={<Thermometer size={24} />}
            />
            <SensorTile title="Flow" value={waterFlow} unit="mL/min" icon={<Drop size={24} />} />
            <SensorTile
                title="Volume"
                value={waterVolume}
                unit="milliliter"
                icon={<Drop size={24} weight="fill" />}
            />
            <SensorTile
                title="Barometric"
                value={barometric}
                unit="mbar"
                icon={<Cloud size={24} />}
            />
            <SensorTile title="Depth" value={waterDepth} unit="meter" icon={<Anchor size={24} />} />
        </TileCollection>
    );
};

export const Monitoring = () => {
    const { min, max } = useScreen();
    const { status, reload } = useStatusUpdate();
    return (
        <div>
            <h1 tw="text-display text-primary p-8 pb-0 mx-auto w-full max-w-screen-xl">
                Monitoring
            </h1>
            <main
                tw="grid w-full max-w-screen-xl gap-8 mx-auto p-8"
                css={
                    min`md`
                        ? { gridTemplateColumns: "1fr min-content" }
                        : { gridTemplateColumns: "1fr" }
                }>
                <div tw="grid gap-8 content-start">
                    <ValveStatus />
                    {max`md` && <Tiling columns={2} />}
                    <StateTable />
                </div>
                {min`md` && <Tiling columns={min`xl` ? 2 : 1} />}
            </main>
        </div>
    );
};
