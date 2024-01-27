import { ReactElement, ReactNode } from "react";

export interface TListPropsWithRenderProp<T extends unknown> {
    items: T[];
    renderItem: (item: T, index: number) => ReactNode;
    renderContainer?: (props: { children: ReactNode; className?: string }) => ReactElement;
    className?: string;
}

export type TListProps<T> = TListPropsWithRenderProp<T>;

export const List = <T extends unknown>({
    items,
    className,
    renderItem,
    renderContainer = props => <ul {...props} />,
}: TListProps<T>) => {
    return renderContainer({
        className: className,
        children: items.map(renderItem),
    });
};
