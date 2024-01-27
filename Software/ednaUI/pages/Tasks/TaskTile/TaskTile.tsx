//
// ──────────────────────────────────────────────── I ──────────
//   :::::: T I L E : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────
//

import { Link } from "react-router-dom";
import tw, { styled } from "twin.macro";
import { NotePencil } from "phosphor-react";
import { truncate } from "lodash";

const Tile = styled(Link)`
    ${tw`relative grid h-56 p-4 rounded-md bg-white text-subtitle text-secondary shadow-2xl hover:(cursor-pointer bg-gray-800)`}
    transition: hover 0.5s;
    grid-template-columns: max-content 1fr;
    grid-template-rows: repeat(3, min-content);
`;

export type TaskTileProps = {
    id?: string;
    name?: string;
    createdAt?: string;
    notes?: string;
    active?: boolean;
};

const Ping = () => (
    <span tw="absolute flex -right-1.5 -top-1.5">
        <span tw="animate-ping absolute h-3 w-3 rounded-full bg-accent opacity-75" />
        <span tw="h-3 w-3 rounded-full bg-accent" />
    </span>
);

export const TaskTile = ({
    id,
    createdAt: created,
    name,
    notes: description,
    active = false,
}: TaskTileProps) => {
    return (
        <Tile to={`/tasks/${id}`} className="group">
            {active && <Ping />}
            <div tw="text-overline pb-2">CREATED {created}</div>
            <div tw="text-title font-bold leading-none col-span-full text-primary group-hover:(text-teal-400)">
                {name}
            </div>
            <div tw="text-sm text-secondary py-4 col-span-full">
                {truncate(description, { length: 80 })}
            </div>
            <NotePencil
                size={24}
                tw="justify-self-end self-end col-span-full group-hover:(text-accent)"
            />
        </Tile>
    );
};
