import { throttle } from "lodash";
import { useRef, useEffect, Ref, ReactElement } from "react";
import { styled } from "twin.macro";

const Container = styled.div<{ perspective: number }>`
    perspective: ${props => props.perspective}px;
`;

type ParallaxProps = {
    maxAngle?: number;
    perspective?: number;

    ref?: Ref<HTMLDivElement>;
    className?: string;
};

export type ParallaxPropsWithRender<T> = ParallaxProps & {
    render: (ref: Ref<T>) => ReactElement;
    children?: never;
};

export type ParallaxWithChildren<T> = ParallaxProps & {
    children: (ref: Ref<T>) => ReactElement;
    render?: never;
};

export function Parallax<T extends HTMLElement>({
    maxAngle = 3,
    perspective = 200,
    ref: externalRef,
    className,
    ...props
}: ParallaxWithChildren<T> | ParallaxPropsWithRender<T>) {
    const ref = useRef<T>(null);

    useEffect(() => {
        const target = ref.current;
        if (!target) return;

        // For smooth animation
        target.style.willChange = "transform";
        target.style.transition = "transform 0.5s";

        // Calculate X and Y angle to rotate. This is done by measure relative mouse location from the center of the target node
        const moveHandler = throttle((event: MouseEvent) => {
            const x_origin = target.getBoundingClientRect().left + target.offsetWidth / 2;
            const y_origin = target.getBoundingClientRect().top + target.offsetHeight / 2;

            const x_prime = event.clientX - x_origin;
            const y_prime = event.clientY - y_origin;

            const x = x_prime / (target.offsetWidth / 2);
            const y = y_prime / (target.offsetHeight / 2);

            target.style.transform = `rotateX(${y * -maxAngle}deg) rotateY(${x * maxAngle}deg)`;
        }, 100);

        // Return node to original rotation;
        const leaveHandler = () => {
            timer = setTimeout(() => {
                target.style.transform = "";
            }, 1000);
        };

        // Clear previous setTimeout to stop target from return to original position
        let timer: NodeJS.Timeout;
        const enterHandler = () => clearTimeout(timer);

        const handlers = [
            ["mouseenter", enterHandler],
            ["mousemove", moveHandler],
            ["mouseleave", leaveHandler],
        ] as const;

        handlers.forEach(([e, h]) => target.addEventListener(e, h));
        return () => handlers.forEach(([e, h]) => target.removeEventListener(e, h));
    }, [maxAngle]);

    return (
        <Container perspective={perspective} className={className} ref={externalRef}>
            {/* Typescript is super smart */}
            {props.render ? props.render(ref) : props.children(ref)}
        </Container>
    );
}
