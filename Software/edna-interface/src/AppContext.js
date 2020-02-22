import { h, createContext } from "preact";


const AppContext = createContext();
// const AppContextProvider = ({ children, props }) => (
// 	<AppContext.Provider {...props}>
// 		{children}
// 	</AppContext.Provider>
// );

const AppContextProvider = AppContext.Provider;


export { AppContext, AppContextProvider };