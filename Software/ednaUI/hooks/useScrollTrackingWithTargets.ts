import { useEffect, useState, useRef } from "react";
import { minBy } from "lodash";

import { useScrollTracking } from "hooks";

import { notNullish } from "lib";

export const useScrollTrackingWithTargets = (
    scroller: string,
    targetIds: string[],
    options: {
        throttleMs: number;
        offset: number;
    } = {
        throttleMs: 200,
        offset: 50,
    }
) => {
    const [match, setMatch] = useState(0);
    const [pause, setPause] = useState(false);
    const scrollData = useScrollTracking(scroller, options.throttleMs);
    const refs = useRef<HTMLElement[]>([]);

    // Get references to all target doms
    useEffect(() => {
        refs.current = targetIds.map(id => document.getElementById(id)).filter(notNullish);
    }, [targetIds]);

    useEffect(() => {
        // Don't track if we are pausing or at the bottom of the screen
        const bottomEdge = scrollData.top + window.innerHeight === scrollData.height;
        if (pause || bottomEdge) {
            return;
        }

        // Find index of element that is closest to the top
        const targets = refs.current;
        const closest = minBy(targets, target =>
            Math.abs(target.getBoundingClientRect().top - options.offset)
        );
        setMatch(closest ? targets.indexOf(closest) : 0);
    }, [scrollData, pause, options.offset]);

    return { match, pause, setPause, setMatch } as const;
};
