import { useCallback, useEffect, useState } from "react";
import { useSelector } from "react-redux";

import { getStatusUpdate } from "root@redux/actions";
import { useAppDispatch } from "root@redux/store";

type StatusText = "fulfilled" | "pending" | "rejected";
export function useStatusUpdate() {
    const dispatch = useAppDispatch();
    const status = useSelector(state => state.status);
    const [statusText, setStatusText] = useState<StatusText>("fulfilled");

    // Alows client to reload status whenever
    const reload = useCallback(() => {
        setStatusText("pending");
        return dispatch(getStatusUpdate());
    }, [dispatch]);

    // Synchronizing status changes
    useEffect(() => {
        if (statusText === "pending" || status.rejects >= 3) {
            return;
        }

        function fetchStatus() {
            const promise = reload();
            const timer = setTimeout(() => promise.abort(), 1000);
            promise
                .then(() => {
                    clearTimeout(timer);
                    setStatusText("fulfilled");
                })
                .catch(() => {
                    setStatusText("rejected");
                });

            return timer;
        }

        const timer = setInterval(() => fetchStatus(), 1000);
        return () => clearTimeout(timer);
    }, [reload, statusText, status.rejects]);

    return { status, reload };
}
