import "twin.macro";

export function NotFound() {
    return (
        <div tw="grid place-items-center w-full h-full flex-grow">
            <div tw="text-display text-primary animate-bounce">404 Not Found</div>
        </div>
    );
}
