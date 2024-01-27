import { createReducer, isAnyOf } from "@reduxjs/toolkit";
import { random, times, transform } from "lodash";

import { StatusInStore, TaskCollectionInStore } from "root@redux/models";
import {
    getStatusUpdate,
    getTaskCollection,
    createTask,
    scheduleTask,
    unscheduleTask,
    setLoadingScreen,
    getTask,
    updateTask,
} from "root@redux/actions";

import BUILD_META from "app/build.json";

//
// ────────────────────────────────────────────────────── I ──────────
//   :::::: H E L P E R S : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────────
//

//
// ──────────────────────────────────────────────────────── II ──────────
//   :::::: R E D U C E R S : :  :   :    :     :        :          :
// ──────────────────────────────────────────────────────────────────
//

export const status = createReducer<StatusInStore>({ rejects: 0 }, builder =>
    builder
        .addCase(getStatusUpdate.fulfilled, (_, action) => {
            return { ...action.payload, rejects: 0 };
        })
        .addCase(getStatusUpdate.rejected, state => {
            state.rejects++;
        })
);
const makeMock = (id: string, status: number) => ({
    id,
    createdAt: Math.floor(Date.now() / 1000),
    name: "DEMO",
    notes: "Lorem ipsum dolor sit amet consectetur adipisicing elit. Veniam, debitis",
    status,
    valves: [],
    flushTime: 0,
    flushVolume: 0,
    sampleTime: 0,
    sampleVolume: 0,
    samplePressure: 0,
    preserveTime: 0,
    schedule: Math.floor(Date.now() / 1000),
    scheduleOnReceived: true,
    timeBetween: 10,
    waterTemp: 4,
});

const initialTaskCollection: TaskCollectionInStore =
    BUILD_META.env === "development"
        ? transform(
              times(5, () => random(2147483647)),
              (result, id) => (result[id] = makeMock(id.toString(), Number(id) % 2))
          )
        : {};

export const taskCollection = createReducer(initialTaskCollection, builder =>
    builder
        .addCase(getTaskCollection.fulfilled, (_, { payload: collection }) => {
            return collection;
        })
        .addMatcher(
            isAnyOf(
                createTask.fulfilled,
                getTask.fulfilled,
                scheduleTask.fulfilled,
                unscheduleTask.fulfilled
            ),
            (state, { payload: task }) => {
                return { ...state, [task.id]: task };
            }
        )
);

export const loadingScreen = createReducer<"showing" | "hiding">("hiding", builder =>
    builder.addCase(setLoadingScreen, (_, action) => action.payload)
);
