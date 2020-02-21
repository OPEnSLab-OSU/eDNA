import { useState } from "preact/hooks";

export function useFormChange() {
	const [data, setData] = useState({});
	const handleChange = (e) => {
		setData({ ...data, [e.target.getAttribute("name")]: e.target.value });
	};

	return [data, handleChange];
}