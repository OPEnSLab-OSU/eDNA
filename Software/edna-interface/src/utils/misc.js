
export function sleep(ms) {
	return new Promise(resolve => setTimeout(resolve, ms));
}

export async function sendRequestWithTimeout(ms, url, options = {}) {
	return new Promise(function(resolve, reject) {
		var xhr = new XMLHttpRequest();
		xhr.timeout = ms;
		xhr.responseType = options.type || "text";
		xhr.open(options.method || "GET", url);
		xhr.onload = function() {
			if (xhr.status >= 200 && xhr.status <= 400) {
				resolve({
					status: xhr.status,
					statusText: xhr.statusText,
					data: xhr.response
				});
			} else {
				reject({
					error: true,
					status: xhr.status,
					statusText: xhr.statusText
				});
			}
			
		};

		xhr.ontimeout = _ => {
			reject({ timeout: true });
		};

		xhr.onerror = _ => {
			reject({ error: true });
		};

		xhr.send();
	});
}

export function timeoutPromise(ms, promise) {
	const timeout = new Promise((resolve, reject) => {
		const id = setTimeout(() => {
			clearTimeout(id);
			reject("Timed out in " + ms + "ms.");
		}, ms);
	});

	// Returns a race between our timeout and the passed in promise
	return Promise.race([
		promise,
		timeout
	]);
}

export function fetchWithTimeout(ms, resource, init) {
	return timeoutPromise(ms, fetch(resource, init));
}
