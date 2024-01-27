import { useState, useEffect, useCallback } from "react";

import { base } from "app/http";

export type ReturnStatus = "ready" | "pending" | "fail" | "success";
export type Result<T> = { payload?: T; status: ReturnStatus };
export type Return<T> = [result: Result<T>, reload: () => void];

export function useAPI<T>(url: string, options?: RequestInit): Return<T> {
    const [result, setResult] = useState<Result<T>>({ status: "ready" });

    // useCallback to keep referencial equality between each render.
    // Prevents infinite loop in useEffect. The function is defferent only if url or type changes
    const reload = useCallback(async () => {
        setResult(result => ({ ...result, status: "pending" }));
        try {
            const response = await fetch(new URL(url, base).toString(), options);
            const payload = (await response.json()) as T;
            setResult({ payload, status: "success" });
        } catch (e) {
            setResult({ status: "fail" });
        }
    }, [url, options]);

    useEffect(() => {
        reload();
    }, [reload]);

    return [result, reload];
}
