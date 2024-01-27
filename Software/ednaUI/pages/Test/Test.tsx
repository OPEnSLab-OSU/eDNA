import tw, { TwStyle } from "twin.macro";
import { useEffect } from "react";
import { useDispatch, useSelector } from "react-redux";

import { Button } from "components/units/Button";
import { getStatusUpdate, getTaskCollection, setLoadingScreen } from "root@redux/actions";
import { ReturnStatus, useAPIWithTimeout } from "hooks";
import { StatusServer } from "root@redux/models";

type ConnectionBadgeProps = {
    status: ReturnStatus;
};

const ConnectionBadge = ({ status }: ConnectionBadgeProps) => {
    const mapStatusToStyles: Record<ReturnStatus & string, { text: string; style: TwStyle }> = {
        ready: { text: "Ready", style: tw`bg-accent` },
        pending: { text: "Loading", style: tw`bg-accent` },
        success: { text: "Success", style: tw`bg-accent` },
        fail: { text: "Failed to connect", style: tw`bg-red-500` },
    };

    return (
        <div tw="grid grid-flow-col gap-4 items-center">
            <span tw="rounded-full h-4 w-4" css={mapStatusToStyles[status].style} />
            <span>{mapStatusToStyles[status].text}</span>
        </div>
    );
};

export const Test = () => {
    const { result, pause, setPause } = useAPIWithTimeout<StatusServer>("api/status");
    const dispatch = useDispatch();
    const loading = useSelector(state => state.loadingScreen);
    const status = useSelector(state => state.status);

    useEffect(() => {
        console.log("Status", status);
    }, [status]);

    return (
        <div tw="max-w-screen-xl mx-auto p-8 grid w-full h-full place-items-center">
            <button onClick={() => dispatch(setLoadingScreen("showing"))}>oh yeah!!</button>
            {loading === "showing" && <div>I‘m Loading YAY!!!</div>}

            {/* <Button text="Pause" onClick={() => setPause(!pause)} />
            <ConnectionBadge status={result.status} /> */}

            {result.payload &&
                Object.entries(result.payload).map(([key, value]) => (
                    <p key={key}>
                        {key}
                        {value}
                    </p>
                ))}

            <Button onClick={() => dispatch(getStatusUpdate())} text="Get Status" />
            <Button onClick={() => dispatch(getTaskCollection())} text="Get all tasks" />
        </div>
    );
};
