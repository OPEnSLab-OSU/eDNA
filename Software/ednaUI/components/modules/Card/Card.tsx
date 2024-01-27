import tw from "twin.macro";
import { ReactNode } from "react";

type TCardProps = {
    title?: string;
    className?: string;
    children?: ReactNode;
};

const Title = tw.div`pb-4 text-title font-bold tracking-tight text-primary`;
export const Card = ({ title, className, children }: TCardProps) => {
    return (
        <div tw="p-8 rounded-lg" className={className}>
            {title && <Title>{title}</Title>}
            <div>{children}</div>
        </div>
    );
};
