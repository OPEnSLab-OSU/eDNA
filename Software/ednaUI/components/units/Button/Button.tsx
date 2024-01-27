import { ComponentPropsWithoutRef, forwardRef, ReactNode } from "react";
import tw from "twin.macro";

export type ButtonProps = ComponentPropsWithoutRef<"button"> & {
    icon?: ReactNode;
    iconRight?: boolean;
    text?: string;
};

const Spacing = ({ hidden }: { hidden?: boolean }) => (hidden ? null : <span tw="w-2"></span>);

const Title = tw.span`w-full text-center whitespace-nowrap`;
export const Button = forwardRef<HTMLButtonElement, ButtonProps>(
    ({ icon, iconRight, text, ...props }, ref) => {
        return (
            <button
                ref={ref}
                type="button"
                tw="flex items-center p-3 text-sm text-primary bg-white rounded shadow min-w-min
                transition duration-200
                hover:(shadow-lg) 
                disabled:(text-secondary bg-transparent border border-secondary shadow-none)
                disabled:(transform-none transition-none)"
                {...props}>
                {icon && !iconRight && <span className="icon">{icon}</span>}
                {text && icon && <Spacing hidden={!!iconRight} />}
                {text && <Title>{text}</Title>}
                {text && icon && <Spacing hidden={!iconRight} />}
                {icon && iconRight && <span className="icon">{icon}</span>}
            </button>
        );
    }
);

Button.displayName = "Button";
