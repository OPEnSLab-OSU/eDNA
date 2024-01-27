import { objectToQueryString } from "lib";

export const base = new URL("http://192.168.1.1");

interface HTTPResponse<T> extends Omit<Response, "json" | "text"> {
    data?: T;
}

export async function http<T>(request: RequestInfo): Promise<HTTPResponse<T>> {
    const response = await fetch(request);
    const parsedResponse: HTTPResponse<T> = response;
    parsedResponse.data = await response.json();
    console.log(parsedResponse.data);
    return parsedResponse;
}

//
// ────────────────────────────────────────────────────────────────── I ──────────
//   :::::: A P I   F U N C T I O N S : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────────────────────
//

export type TimeoutError = "TimeoutError";
export const TimeoutError: TimeoutError = "TimeoutError";

export interface APIResponseWtihData<T> {
    error?: TimeoutError | string;
    success?: string;
    payload?: T;
}

export interface APIResponse {
    error?: TimeoutError | string;
    success?: string;
}

export class APIBuilder {
    private controller?: AbortController;
    private timeout!: number;

    constructor(private path: string, private options: RequestInit) {
        this.path = path;
        this.options = options;
    }

    withTimeout(ms = 3000) {
        this.timeout = ms;
        this.controller = new AbortController();
        this.options.signal = this.controller.signal;
        return this;
    }

    withJson(payload: unknown) {
        this.options.body = JSON.stringify(payload);
        return this;
    }

    withQuery(params: Record<string, unknown>) {
        this.path += `?${objectToQueryString(params)}`;
        return this;
    }

    async send(): Promise<APIResponse>;
    async send<T>(): Promise<APIResponseWtihData<T>>;
    async send<T>(): Promise<APIResponseWtihData<T> | APIResponse> {
        const controller = this.controller;
        if (controller) {
            setTimeout(() => controller.abort(), this.timeout - 10);
        }

        const path = new URL(this.path, base);
        try {
            // Normalize http status code errors (anything that's not in 200-299 range)
            const response = await http<APIResponseWtihData<T>>(
                new Request(path.toString(), this.options)
            );

            console.log(response);

            if (!response.ok) {
                return { error: response.statusText };
            }

            // Normalize our server error
            const data = response.data;
            if (data && data.error) {
                return { error: data.error };
            } else {
                return { ...data };
            }
        } catch (e) {
            // Normallize TimeoutError and NetworkError
            if (e.name === "AbortError") return { error: TimeoutError };
            return { error: e.name };
        }
    }
}

export function get(path: string, options: RequestInit = {}) {
    return new APIBuilder(path, { ...options, method: "get" });
}

export function post(path: string, options: RequestInit = {}) {
    return new APIBuilder(path, { ...options, method: "post" });
}
