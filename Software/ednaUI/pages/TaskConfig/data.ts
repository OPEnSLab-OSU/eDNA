import z, { string, number, object } from "zod";

const ValveSchema = number().min(0).max(23);

export const FormSchema = object({
    name: string().max(24),
    date: string().refine(str => str.match(/\d{4}-\d{2}-\d{2}/), {
        message: "Date doesn't match the required form at: yyyy-mm-dd",
    }),
    time: string().refine(str => str.match(/\d{1,2}:\d{1,2}/), {
        message: "Time doesn't match the required format: hh:mm (24hr)",
    }),
    valves: string()
        .nonempty()
        .refine(
            s => s.split(",").every(n => n.split("-").every(n => !isNaN(Number(n)) || n == "-")),
            {
                message: "Input contains non-numeric character or doesn't follow the format",
            }
        )
        .refine(
            s =>
                s
                    .split(",")
                    .every(n => n.split("-").every(n => ValveSchema.safeParse(Number(n)).success)),
            {
                message: "Valve number must be >= 0 and <= 23",
            }
        )
        .refine(
            s =>
                Array.from(s.matchAll(/(\d*-\d*)/g)).every(m =>
                    RegExp(/(\d+-\d+)/g).test(m.toString())
                ),
            {
                message: "Range missing bound",
            }
        )
        .refine(
            s =>
                Array.from(s.matchAll(/(\d+-\d+)/g)).every(
                    m =>
                        Number([...m.toString().matchAll(/(\d+(?=-))-(\d+)/g)][0][1]) <
                        Number([...m.toString().matchAll(/(\d+(?=-))-(\d+)/g)][0][2])
                ),
            {
                message: "Larger bound is first",
            }
        ),
    timeBetween: number().min(0),
    notes: string().optional(),
    flushTime: number().min(0),
    sampleTime: number().min(0),
    sampleVolume: number().min(0),
    preserveTime: number().min(0),
    waterTemp: number().min(0),
});

export type FormValues = z.infer<typeof FormSchema>;

export type FieldProps = {
    name: keyof FormValues;
    label: string;
    type?: "string" | "number" | "date" | "time" | "button";
    sublabel?: string;
    helperText?: string;
};

export const generalFields: FieldProps[] = [
    { name: "name", label: "Task Name", sublabel: "Name unique to this task" },
    {
        name: "date",
        type: "date",
        label: "Schedule Date",
        sublabel: "Date when to execute this task",
        helperText: "Format: yyyy-mm-dd",
    },
    {
        name: "time",
        type: "time",
        label: "Schedule Time",
        sublabel: "Time of the day to execute this task",
        helperText: "Format: hh:mm (pm|am)",
    },
    {
        name: "notes",
        label: "Notes",
        sublabel: "Optional note to remember this task",
    },
];

export const valveFields: FieldProps[] = [
    {
        name: "valves",
        label: "Valves",
        sublabel: "Valves asigned to this task",
        helperText: "Comma-separated valve numbers & ranges: eg. 1,3-8,21",
    },
    {
        name: "timeBetween",
        label: "Time Between",
        sublabel: "Time until next valve",
        type: "number",
        helperText: "Unit: second",
    },
];

export const flushFields: FieldProps[] = [
    { name: "flushTime", type: "number", label: "Flush Time", helperText: "Unit: second" },
    // { name: "flushVolume", type: "number", label: "Flush Volume" },
];

export const sampleFields: FieldProps[] = [
    { name: "sampleTime", type: "number", label: "Sample Time", helperText: "Unit: second" },
    {
        name: "sampleVolume",
        type: "number",
        label: "Sample Volume",
        helperText: "Unit: milliliter",
    },
];

export const preserveFields: FieldProps[] = [
    { name: "preserveTime", type: "number", label: "Preserve Time", helperText: "Unit: second" },
];

export const TemperatureFields: FieldProps[] = [
    { name: "waterTemp", type: "number", label: "Water Temperature", helperText: "Unit: celsius" },
];
export type ConfigSectionName = "general" | "valves" | "flush" | "sample" | "preserve"|"temperature";
export const configFields: Record<ConfigSectionName, { title: string; fields: FieldProps[] }> = {
    general: { title: "General", fields: generalFields },
    valves: { title: "Valves", fields: valveFields },
    flush: { title: "Flush", fields: flushFields },
    sample: { title: "Sample", fields: sampleFields },
    preserve: { title: "Preserve", fields: preserveFields },
    temperature: {title: "Temperature", fields: TemperatureFields},
};

