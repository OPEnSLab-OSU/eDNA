import { ReactNode } from "react";
import { Activity, Book, List, Wrench, Lightning } from "phosphor-react";
import { NavLink } from "react-router-dom";
import tw, { styled } from "twin.macro";

interface NavButtonProps {
    to: string;
    text?: string;
    icon: ReactNode;
}

const StyledNavButton = styled(NavLink)`
    ${tw`flex items-center px-8 text-sm tracking-wide text-secondary lg:px-12 hover:bg-background`}
`;

const NavButton = ({ to, text, icon }: NavButtonProps) => {
    return (
        <StyledNavButton to={to} activeStyle={tw`border-r-4 border-accent text-primary`}>
            <div tw="grid grid-flow-col items-center gap-4">
                {icon}
                <div tw="hidden lg:block">{text}</div>
            </div>
        </StyledNavButton>
    );
};

export const Sidebar = () => {
    return (
        <div
            tw="grid flex-shrink-0 h-screen overflow-y-scroll z-10 shadow-xl"
            css={{ gridTemplateRows: "88px 64px", gridAutoRows: "100px" }}>
            <div
                tw="hidden grid-flow-col gap-4 text-xl lg:grid items-center justify-center mt-8"
                css={{ letterSpacing: "1rem" }}>
                <span tw="px-2 py-2 tracking-normal text-white rounded-md bg-accent">E</span>
                <span>DNA</span>
            </div>
            <div tw="flex items-center w-full text-3xl text-white bg-accent justify-center lg:hidden">
                E
            </div>
            <div />
            <NavButton text="Documentation" to="/documentation" icon={<Book size={24} />} />
            <NavButton text="Monitoring" to="/monitoring" icon={<Activity size={24} />} />
            <NavButton text="Tasks" to="/tasks" icon={<List size={24} />} />
            <NavButton text="Utilities" to="/utilities" icon={<Wrench size={24} />} />
            {process.env.NODE_ENV === "development" && (
                <NavButton text="Test" to="/test" icon={<Lightning size={24} />} />
            )}
        </div>
    );
};
