import { NavLink } from "react-router-dom";
import tw from "twin.macro";

type ConfigListItemProps = {
    text: string;
    index: number;
    hash?: string;
    onClick: (match: number) => void;
};

export const ConfigListItem = ({
    text,
    hash = `#${text.toLowerCase()}`,
    index,
    onClick,
}: ConfigListItemProps) => {
    const clickHandler = () => {
        onClick(index);
        const page = document.querySelector("#page");
        const target = document.querySelector(hash);
        if (page && target) {
            const top = target.getBoundingClientRect().top - 32;
            page.scrollBy({ top, behavior: "smooth" });
        }
    };

    return (
        <li tw="flex">
            <NavLink
                tw="text-right text-secondary py-2 w-full hover:(text-primary cursor-pointer)"
                to={hash}
                activeStyle={tw`text-primary`}
                isActive={(_, loc) => (hash === "#general" && !loc.hash) || loc.hash === hash}
                onClick={clickHandler}>
                {text}
            </NavLink>
        </li>
    );
};
