import { combineReducers, configureStore, Middleware } from "@reduxjs/toolkit";
import { useDispatch } from "react-redux";

import {} from "root@redux/actions";
import * as reducers from "root@redux/reducers";

const logger: Middleware = _ => next => action => {
    console.log("%cRedux Action", "font-weight:bold;color:teal;", action.type);
    next(action);
};

const rootReducer = combineReducers(reducers);
export const store = configureStore({
    reducer: rootReducer,
    middleware: getDefaultMiddleware => getDefaultMiddleware().concat(logger),
});

export type RootState = ReturnType<typeof rootReducer>;
export type AppDispatch = typeof store.dispatch;
export const useAppDispatch = () => useDispatch<AppDispatch>();
