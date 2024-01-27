//
// ────────────────────────────────────────────────────── I ──────────
//   :::::: H E L P E R S : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────────
//

export function notNullish<T>(value: T | null | undefined): value is T {
    return value !== null && value !== undefined;
}

export function isNullish<T>(value: T | null | undefined): value is null | undefined {
    return !notNullish<T>(value);
}

export function pick<T, K extends keyof T>(obj: T, ...keys: K[]): Pick<T, K> {
    const entries = keys.map(key => [key, obj[key]]);
    return Object.fromEntries(entries);
}

// ────────────────────────────────────────────────────────────────────────────────
// Combine two arrays into an array of tuple. See Python's zip for details.
// ────────────────────────────────────────────────────────────────────────────────
export const zip = <T, K>(a: T[], b: K[]): [T, K][] => a.map((k, i) => [k, b[i]]);

// ────────────────────────────────────────────────────────────────────────────────
// Convert object to query string
// ────────────────────────────────────────────────────────────────────────────────
export const objectToQueryString = (obj: { [key: string]: unknown }) => {
    return Object.keys(obj)
        .map(key => `${key}=${obj[key]}`)
        .join("&");
};

export const partition = <T extends unknown>(ary: T[], predicate: (elem: T) => boolean) => {
    const a: T[] = [];
    const b: T[] = [];
    ary.forEach(e => (predicate(e) ? a.push(e) : b.push(e)));
    return [a, b] as const;
};

// ────────────────────────────────────────────────────────────────────────────────
// The server sends tasklist as an array. This method convert array of objects to
// a single level object using the given key as direct properties
// ────────────────────────────────────────────────────────────────────────────────
export const arrayToObject = <T extends Record<string, unknown>>(array: T[], key: keyof T) => {
    const obj: Record<string, T> = {};
    array.forEach(item => {
        obj[item[key] as string] = item;
    });
    return obj;
};

export const sum = (numbers: (number | null | undefined)[]): number => {
    return numbers.reduce((a, b) => (a ?? 0) + (b ?? 0), 0) as number;
};

export const toDateString = (schedule?: number) => {
    const date = new Date((schedule ?? 0) * 1000);
    const components = [date.getFullYear(), date.getMonth() + 1, date.getDate()];
    return components.map(c => c.toString().padStart(2, "0")).join("-");
};

export const toTimeString = (schedule?: number) => {
    const date = new Date((schedule ?? 0) * 1000);
    const components = [date.getHours(), date.getMinutes()];
    return components.map(c => c.toString().padStart(2, "0")).join(":");
};

export const secondToTimeComponents = (seconds?: number) => {
    seconds = seconds ?? 0;
    const day = Math.floor(seconds / (3600 * 24));
    const hour = Math.floor((seconds % (3600 * 24)) / 3600);
    const min = Math.floor((seconds % 3600) / 60);
    const sec = Math.floor(seconds % 60);
    return { day, hour, minute: min, second: sec } as const;
};

export type UnionToIntersection<U> = (U extends unknown ? (k: U) => void : never) extends (
    k: infer I
) => void
    ? I
    : never;

export type PickAndFlatten<T, K extends keyof T> = UnionToIntersection<T[K]>;
