import { Dictionary } from "@reduxjs/toolkit";
import z, { number, string, array, object, boolean } from "zod";
//
// ──────────────────────────────────────────────── I ──────────
//   :::::: K E Y S : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────
//

export const KEYS = {
    SENSOR_BARO: "barometric",
    SENSOR_DEPTH: "waterDepth",
    SENSOR_FLOW: "waterFlow",
    SENSOR_PRESSURE: "pressure",
    SENSOR_TEMP: "temperature",
    SENSOR_VOLUME: "waterVolume",
    STATE_CURRENT_NAME: "stateCurrentName",
    STATE_ID: "stateId",
    STATE_INDEX: "stateIndex",
    STATE_NAME: "stateName",
    TIME_UTC: "timeUTC",
    VALVE_CURRENT: "valveCurrent",
    VALVE_FLUSH_TIME: "valveFlushTime",
    VALVE_FLUSH_VOLUME: "valveFlushVolume",
    VALVE_GROUP: "valveGroup",
    VALVE_ID: "valveId",
    VALVE_PRESERVE_TIME: "valvePreserveTime",
    VALVE_SAMPLE_PRESSURE: "valveSamplePressure",
    VALVE_SAMPLE_TIME: "valveSampleTime",
    VALVE_SAMPLE_VOLUME: "valveSampleVolume",
    VALVE_SCHEDULE: "valveSchedule",
    VALVE_STATUS: "valveStatus",
    VALVES: "valves",
    VALVES_COUNT: "valvesCount",
    TASK_NAME: "name",
    TASK_STATUS: "status",
    TASK_SCHEDULE: "schedule",
    TASK_VALVES: "valves",
    TASK_TIME_BETWEEN: "timeBetween",
    TASK_NOTES: "notes",
};

//
// ────────────────────────────────────────────────────── II ──────────
//   :::::: H E L P E R S : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────────
//

//
// ──────────────────────────────────────────────── III ─────────
//   :::::: T A S K : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────
//

// ────────────────────────────────────────────────────────────────────────────────
// This schema is used to ensure correct Task object shape between server and client
// ────────────────────────────────────────────────────────────────────────────────
// This is where we define the structure of our JSON payload. In a more robust env,
// one would send over a schema and use an editor plugin to strongly-type out the
// object but this should do for now.

// prettier-ignore
export const TaskServerSchema = object({
    id: string(),
    name: string(),
    createdAt: number()
        .int()
        .min(0),
    status: number()
        .int()
        .min(0),
    schedule: number()
        .int()
        .min(0),
    valves: array(number().int()),
    timeBetween: number()
        .min(0),
    scheduleOnReceived: boolean(),
    notes: string(),
    flushTime: number()
        .min(0),
    sampleTime: number()
        .min(0),
    sampleVolume: number()
        .min(0),
    preserveTime: number()
        .min(0),
    waterTemp: number()
        .min(0),
});

export type TaskServerStatus = "inactive" | "active" | "completed" | "missed";
export type TaskServer = z.infer<typeof TaskServerSchema>;
export type TaskCollectionInStore = Dictionary<TaskServer>;
export const mapTaskStatusToString: Record<number, TaskServerStatus> = {
    0: "inactive",
    1: "active",
    2: "completed",
    3: "missed",
};

//
// ──────────────────────────────────────────────────── IV ──────────
//   :::::: S T A T U S : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────
//

// prettier-ignore
export const StatusServerSchema = object({
    currentState: string(),
    currentTask: string().nullable(),
    valves: array(number()),
    utc: number()
        .min(0),
    pressure: number()
        .min(0),
    temperature: number()
        .min(0),
    barometric: number()
        .min(0),
    waterVolume: number()
        .min(0),
    waterFlow: number()
        .min(0),
    waterDepth: number()
        .min(0),
    lowBattery: boolean(),
    sampleVolume: number()
        .min(0),
    cutOffPressure: number()
        .min(0)
});

export type StatusServer = z.infer<typeof StatusServerSchema>;
export type StatusInStore = Partial<StatusServer> & { rejects: number };

//
// ────────────────────────────────────────────────── V ──────────
//   :::::: V A L V E : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────
//
