import { useReducer, useCallback } from "preact/hooks";
import { sendRequestWithTimeout, sleep } from "../utils/misc.js";

export const STATUS_FETCHING = "FETCHING";
export const STATUS_ERROR = "ERROR";
export const STATUS_TIMEOUT = "TIMEOUT";
export const STATUS_SUCCESS = "SUCCESS";

export const useAPIStatusProvider = (endpoint, options = {}) => {

	const fetching = () => ({ type: STATUS_FETCHING });
	const error = () => ({ type: STATUS_ERROR });
	const timeout = () => ({ type: STATUS_TIMEOUT });
	const success = response => ({ type: STATUS_SUCCESS, response });

	const initialState = {
		status: null,
		response: null,
	};

	const statusReducer = (state = initialState, { type, response } = {}) => {
		switch (type) {
		case STATUS_FETCHING:
			return { status: STATUS_FETCHING };
		case STATUS_ERROR:
			return { status: STATUS_ERROR };
		case STATUS_TIMEOUT:
			return { status: STATUS_TIMEOUT };
		case STATUS_SUCCESS:
			return { status: STATUS_SUCCESS, response };
		default:
			return state;
		}
	};

	const [status, dispatchStatus] = useReducer(statusReducer, initialState);
	const makeRequest = useCallback(async () => {
		try {
			dispatchStatus(fetching());
			const response = await Promise.all([sendRequestWithTimeout(1000, endpoint, options), sleep(50)]);
			dispatchStatus(success(response));
		} catch (e) {
			if (e.timeout) {
				dispatchStatus(timeout());
			} else {
				dispatchStatus(error());
			}
		}
	}, [endpoint, options]);
	return [status, makeRequest];
};
