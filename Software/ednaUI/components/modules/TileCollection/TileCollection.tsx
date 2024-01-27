import tw, { styled } from "twin.macro";
import { ReactNode } from "react";

import { Card } from "components/modules/Card";

type Props = {
    title?: string;
    className?: string;
    children?: ReactNode;
    columns: number;
};

const TileContainer = styled.div<{ columns: number }>`
    ${tw`grid gap-4 w-full text-gray-700 border-gray-200`}
    ${props => `grid-template-columns: repeat(${props.columns}, minmax(12rem, 1fr));`}
`;

export const TileCollection = ({ title = "UNTITLED", children, className, columns }: Props) => {
    return (
        <Card tw="w-full" title={title} className={className}>
            <TileContainer columns={columns}>{children}</TileContainer>
        </Card>
    );
};
