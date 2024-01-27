import "./app.css";
import tw, { theme } from "twin.macro";
import { Route, Redirect, Switch } from "react-router-dom";
import { Provider, useSelector } from "react-redux";
import { BatteryWarning, BatteryFull } from "phosphor-react";

import { Monitoring } from "pages/Monitoring";
import { Tasks } from "pages/Tasks";
import { TaskConfig } from "pages/TaskConfig";

import { Sidebar } from "components/modules/Sidebar";
import { Breadcrumb } from "components/modules/Breadcrumb";

import { ScreenProvider } from "hooks";

import { store as ReduxStore } from "root@redux/store";
import { Test } from "pages/Test";
import { Utilities } from "pages/Utilities";

import { NotFound } from "pages/NotFound";

import BUILD from "./build.json";

const AppContainer = tw.div`flex h-full bg-white debug-screens`;
const PageContainer = tw.div`flex flex-col w-full h-screen overflow-y-scroll bg-background`;
const Toolbar = () => {
    const status = useSelector(state => state.status);
    const lowBattery = status.lowBattery ?? true;

    const { utc } = status;
    const localTime = utc ? new Date(Number(utc * 1000)).toLocaleString() : "----";

    return (
        <div tw="grid grid-flow-col items-center gap-4 py-4 px-8 justify-end text-subtitle text-primary">
            <div tw="grid grid-flow-col gap-2 items-center">
                {lowBattery && "Check Battery Power"}
                {lowBattery ? <BatteryWarning size={24} /> : <BatteryFull size={24} />}
            </div>

            <div>Last Status Update: {localTime}</div>
        </div>
    );
};

const Meta = () => {
    const dirty = BUILD.dirty ? "?" : "";
    const env = BUILD.env[0];
    const meta = `${BUILD.version}-${BUILD.commitNumber}(${BUILD.hash}${dirty}${env})`;
    return (
        <div tw="fixed right-0 bottom-0 p-4 text-sm justify-self-end text-secondary">{`v${meta}`}</div>
    );
};

export const Application = () => (
    <Provider store={ReduxStore}>
        <ScreenProvider screens={theme`screens`}>
            <AppContainer>
                <Sidebar />

                <PageContainer id="page">
                    <Toolbar />
                    <Breadcrumb />

                    <Switch>
                        <Route exact path="/" render={() => <Redirect to="/monitoring" />} />
                        <Route exact path="/404" render={() => <NotFound />} />
                        <Route exact path="/monitoring" render={() => <Monitoring />} />
                        <Route exact path="/tasks" render={() => <Tasks />} />
                        <Route path="/tasks/:taskId" render={() => <TaskConfig />} />
                        <Route path="/utilities" render={() => <Utilities />} />
                        {process.env.NODE_ENV === "development" && (
                            <Route path="/test" render={() => <Test />} />
                        )}
                        <Route path="*" render={() => <Redirect to="/404" />} />
                    </Switch>

                    <Meta />
                </PageContainer>
            </AppContainer>
        </ScreenProvider>
    </Provider>
);
