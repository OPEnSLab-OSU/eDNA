import { forwardRef } from "react";

export type BadgeProps = {
    text?: string;
    className?: string;
};

export const Badge = forwardRef<HTMLDivElement, BadgeProps>(({ text, ...props }, ref) => (
    <div tw="flex items-center justify-center round-full py-2 px-4 text-xs" ref={ref} {...props}>
        {text}
    </div>
));
