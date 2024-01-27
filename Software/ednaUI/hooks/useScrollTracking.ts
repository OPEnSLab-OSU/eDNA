import { useEffect, useState, useRef } from "react";
import { throttle } from "lodash";

export function useScrollTracking(targetId: string, throttleMs = 100) {
    // Useref here to avoid rerendering;
    const ref = useRef<HTMLElement | null>();
    const [data, setData] = useState({
        left: 0,
        top: 0,
        width: 0,
        height: 0,
    });
    // Get the target after DOM has painted
    useEffect(() => {
        ref.current = document.getElementById(targetId);
    }, [targetId]);

    // At this point on we are free to assume target.current exist in useEffect
    useEffect(() => {
        const target = ref.current;
        if (!target) return;

        const track = throttle(() => {
            setData({
                left: target.scrollLeft,
                top: target.scrollTop,
                width: target.scrollWidth,
                height: target.scrollHeight,
            });
        }, throttleMs);

        target.addEventListener("scroll", track);
        return () => {
            target.removeEventListener("scroll", track);
        };
    }, [throttleMs]);

    return data;
}
