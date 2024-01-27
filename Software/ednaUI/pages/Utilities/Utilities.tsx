import tw, { styled } from "twin.macro";
import { MouseEventHandler, Ref, useRef, useState } from "react";
import { useSelector } from "react-redux";
import { useParams } from "react-router-dom";

import { Wrench } from "phosphor-react";

import { any, ZodType } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { FormProvider, useForm } from "react-hook-form";

import { get, post } from "app/http";
import { Parallax } from "components/units/Parallax";
import { ConfigCard, ConfigCardProps } from "pages/TaskConfig/ConfigCard";
import { SubmitCard } from "pages/TaskConfig/SubmitCard";
import { InputField } from "components/units/InputField";
import { useAppDispatch } from "root@redux/store";
import { updatePressure } from "root@redux/actions";
 

const Tile = styled.div`
    ${tw`relative grid h-56 p-4 rounded-md bg-white text-subtitle text-secondary shadow-2xl hover:(cursor-pointer bg-gray-800)`}
    grid-template-rows: min-content min-content 1fr;
`;

const PageContainer = styled.main`
    ${tw`mx-auto max-w-screen-xl grid gap-8 p-8 w-full`}
    grid-template-columns: repeat(auto-fit, minmax(240px, 60ch));
`;

type UtilityProps = {
    name: string;
    description: string;
    onClick: () => void;
};
function Utility({ name, description, onClick }: UtilityProps) {
    return (
        <Parallax
            perspective={800}
            render={(ref: Ref<HTMLDivElement>) => (
                <Tile className="group" ref={ref} onClick={onClick}>
                    <p tw="col-span-full">Tool</p>
                    <div tw="text-title text-primary group-hover:(text-accent)">{name}</div>
                    <p tw="mt-4">{description}</p>
                    <Wrench tw="self-end justify-self-end group-hover:(text-accent)" size={24} />
                </Tile>
            )}
        />
    );
}

type InputUtilityProps = {
    name: string;
    type: "string" | "number" | "date" | "time" | "button";
    description: string;
    onSubmit: (value?: string) => void;
    value?: string;
};
function InputUtility({ name, type, description, onSubmit, value }: InputUtilityProps) {
    const inputRef = useRef<HTMLInputElement>(null);
    return (
        <Parallax
            perspective={800}
            render={(ref: Ref<HTMLDivElement>) => (
                <form
                    tw="grid gap-8 grid-flow-col w-full"
                    css={{ gridTemplateColumns: "minmax(24rem, 30rem) minmax(14rem, 1fr)" }}>
                    <div
                        id={name.toLowerCase()}
                        tw="p-8 grid gap-8 bg-white rounded-xl"
                        className="NumberInput"
                        ref={ref}>
                        <h3 tw="text-title text-primary">{name}</h3>
                        <input tw="form-input text-sm bg-transparent border border-primary rounded" name={name} type={type} value={value} ref={inputRef} />
                        <input type="submit" value="Submit" onClick={() => {
                                    inputRef.current && onSubmit(inputRef.current.value);
                                }} />
                    </div>
                </form>
            )}
        />
    );
}

const HyperFlush = () => (
    <Utility
        name="HyperFlush"
        description={
            "Begin a HyperFlushing sequence. HyperFlush has four stages that runs in thisorder: flush, preload, stop, and idle. The flush stage get rid of excesswater left in the main system. Then the preload stage fills water at the valve connector in order to minimize contemination"
        }
        onClick={() => {
            get("api/preload")
                .withTimeout(1000)
                .send()
                .then(() => {
                    alert("HyperFlush started");
                });
        }}
    />
);

const UpdateRTC = () => (
    <Utility
        name={"Update RTC"}
        description={
            "This utility update the onboard RTC (Real-time-clock) on the eDNA server.This is highly recommended if you move between timezones or when daylightsaving occurs."
        }
        onClick={() => {
            const payload = {
                utc: Math.floor(Date.now() / 1000),
                timezoneOffset: new Date().getTimezoneOffset(),
            };

            post("api/rtc/update").withJson(payload).send();
        }}
    />
);

const ResetValves = () => (
    <Utility
        name={"Reset Valves"}
        description={
            "This utility all valves to its default configuration from the config.js file inthe sdcard"
        }
        onClick={() => {
            const message = "Do you want to reset all the valves to their default configurations";
            window.confirm(message) && get("/api/valves/reset").send();
        }}
    />
);

const BubblePurge = () => (
    <Utility
        name={"Purge Air Bubbles"}
        description={
            "This utility runs alcohol through the system so that there is no more bubbles in the alcohol bag. It turns the pump on and sets the alcohol valve to high."
        }
        onClick={() => {
            get("api/alcohol-debubbler")
                .withTimeout(1000)
                .send()
                .then(() => {
                    alert("Bubble Purge started");
                });
        }}
    />
);

const PressureCutOff = () => {
    const status = useSelector(state => state.status);
    const dispatch = useAppDispatch();

    return (
        <InputUtility
            name={"Set Global Pressure Cut Off"}
            description={"Sets pressure to transition from sample state"}
            onSubmit={(pressure) => {
                if(Number(pressure)<0){
                    alert("Only enter positive values");
                }
                else{
                    dispatch(updatePressure(Number(pressure)));
                    alert("Global pressure cut off has been set");
                }
               
            }}
            type={"number"}
            value={status.cutOffPressure?.toString()}
        />
    );
};
/*
const WaterTemperature = () => {
    const status = useSelector(state => state.status);
    const dispatch = useAppDispatch();

    return (
        <InputUtility
            name={"Set Water temperature"}
            description={"Sends water temperature to server"}
            onSubmit={(waterTemp) => {
                dispatch(updateWaterTemp(Number(waterTemp)));
                alert("Water temperature has been set");
            }}
            type={"number"}
            value={status.waterTemperature?.toString()}
        />
    );
};
*/
export function Utilities() {
    return (
        <PageContainer>
            <h1 tw="text-display text-primary col-span-full ">Utilities</h1>
            <HyperFlush />
            <BubblePurge />
            <UpdateRTC />
            <ResetValves />
        </PageContainer>
    );
}
