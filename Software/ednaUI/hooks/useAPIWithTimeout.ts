import { useCallback, useEffect, useRef, useState } from "react";

import { useAPI } from "hooks";

export function useAPIWithTimeout<T>(url: string, retries = 3) {
    const [result, reload] = useAPI<T>(url);
    const [pause, setPause] = useState(false);
    const attempts = useRef(0);

    const retry = useCallback(() => {
        attempts.current = 0;
        reload();
    }, [reload]);

    useEffect(() => {
        if (result.status === "success") {
            attempts.current = 0;
        }

        if (result.status === "fail") {
            console.log("Status Failed: retrying in 3 seconds");
            setPause(true);
            attempts.current++;

            if (attempts.current !== retries) {
                setTimeout(() => setPause(false), 3000);
            }
        }
    }, [result.status, retries]);

    useEffect(() => {
        const fetchStatus = () => {
            if (pause) {
                console.log("Pausing");
            } else reload();
        };

        const timerId = setInterval(fetchStatus, 1000);
        return () => {
            console.log("Cleared timer");
            clearTimeout(timerId);
        };
    }, [reload, pause]);

    return { result, pause, setPause, retry };
}
