//===============================================================
// [+_+] Constants and Variables
//===============================================================
//@formatter:off
// const host			= "https://testserver--kawinpechetrata.repl.co/";
const host 			= "http://192.168.1.1/";
const stateColors	= ['#173F5F', '#20639B', '#3CAEA3', '#ED553B'];
const TimeoutError = "TimeoutError";
let online 			= false;
let debug 			= true;
let view;
let controller;
let status;
//@formatter:on


function withTimeout(msecs, promise) {
	const timeout = new Promise((resolve, reject) => {
		setTimeout(() => {
			const e = new Error(`request takes more than ${msecs}`);
			e.name = TimeoutError;
			reject(e);
		}, msecs);
	});
	return Promise.race([timeout, promise]);
}

//<editor-fold desc="Scrolling code">
//===============================================================
// [+_+] Scrolling
//===============================================================
// tab: 9, spacebar: 32, pageup: 33, pagedown: 34, end: 35, home: 36, left: 37, up: 38, right: 39, down: 40,
const keys = {9: 1, 32: 1, 33: 1, 34: 1, 35: 1, 36: 1, 37: 1, 38: 1, 39: 1, 40: 1,};

function preventDefault(e) {
	e = e || window.event;
	if (e.preventDefault) {
		e.preventDefault();
	}
	e.returnValue = false;
}

function preventDefaultForScrollKeys(e) {
	return false;
}

function disableScroll() {
	if (window.addEventListener) // older FF
		window.addEventListener('DOMMouseScroll', preventDefault, false);
	document.addEventListener('wheel', preventDefault, {passive: false}); // Disable scrolling in Chrome
	window.onwheel = preventDefault; // modern standard
	window.onmousewheel = document.onmousewheel = preventDefault; // older browsers, IE
	window.ontouchmove = preventDefault; // mobile
	document.onkeydown = preventDefaultForScrollKeys;
}

function enableScroll() {
	if (window.removeEventListener) window.removeEventListener('DOMMouseScroll', preventDefault, false);
	document.removeEventListener('wheel', preventDefault, {passive: false}); // Enable scrolling in Chrome
	window.onmousewheel = document.onmousewheel = null;
	window.onwheel = null;
	window.ontouchmove = null;
	document.onkeydown = null;
}

//</editor-fold>
//<editor-fold desc="Status code">
//===============================================================
// [+_+] Status Updating
//===============================================================
class Status {
	constructor() {
		//@formatter:off
		this.live			= false;
		this.requesting 	= false;
		this.interval 		= 1000;
	}	//@formatter:on

	fetchStatusUpdate() {
		if (this.requesting) {
			return;
		}

		const req = new XMLHttpRequest();
		req.timeout = 3000;
		req.responseType = "json";
		req.onreadystatechange = () => {
			this.requesting = (req.readyState !== XMLHttpRequest.DONE);
		};

		req.onload = () => {
			console.log("Status updated");

			if (online === false) {
				online = true;
				controller.fetchTaskrefsFromServer();
			}
			view.updateStatus(req.response);
			view.connection.textContent = "Online";
			view.connectionIdicator.classList.remove("offline");
			if (this.live) {
				this.timer = setTimeout(() => {this.fetchStatusUpdate();}, this.interval);
			}
		};

		req.ontimeout = () => {
			console.log("Timeout");

			view.connection.textContent = "Offline";
			view.connectionIdicator.classList.add("offline");
			if (this.live) {
				clearTimeout(this.timer);
				this.timer = setTimeout(() => {this.fetchStatusUpdate();}, this.interval);
			}
		};

		req.onerror = (error) => {
			console.log(error);
			console.log("Trying again in 10 secs");

			online = false;
			view.connection.textContent = "Offline";
			view.connectionIdicator.classList.add("offline");
			if (this.live) {
				clearTimeout(this.timer);
				this.timer = setTimeout(() => {this.fetchStatusUpdate();}, 5000);
			}
		};

		req.open("GET", host + "status");
		req.send();
		this.requesting = true;
	}

	beginLiveStatusUpdate(interval) {
		clearTimeout(this.timer);
		this.live = true;
		if (interval) {
			this.interval = interval;
		}
		this.fetchStatusUpdate();
	}

	stopLiveStatusUpdate() {
		clearTimeout(this.timer);
		this.live = false;
		this.requesting = false;
	}
}

//</editor-fold>

//===============================================================
// [+_+] View
//===============================================================
class View {
	constructor() {
		//@formatter:off
		this.main 						= document.getElementById("main");
		this.statusButton 				= document.getElementById("status-button");
		this.statusBar 					= document.getElementById("status-sidebar");
		this.statusTags 				= Array.from(document.querySelectorAll(".status-tag")).reduce((acc, cur) => {
			acc[cur.dataset.name]  		= cur.querySelector("i");
			return acc;
		}, {});

		this.connection					= document.getElementById("connection");
		this.connectionIdicator 		= document.getElementById("connection-indicator");

		this.header 					= document.getElementById("header");
		this.headerButtons = {
			rtc							: document.getElementById("rtc"),
			task						: document.getElementById("tasks"),
			submit						: document.getElementById("submit")
		};

		this.valveOverview 				= document.getElementById("valve-overview");

		this.stateTimeline 				= document.getElementById("state-timeline");
		this.stateConfigContainer 		= document.getElementById("state-config-container");
		this.stateConfigs 				= Array.from(document.getElementsByClassName("state-config"));

		this.taskListings				= document.getElementById("task-listings");
		this.tasks 						= document.getElementsByClassName("task");
		this.taskButtons = {
			view						: document.getElementById("task-view-button"),
			stop						: document.getElementById("task-stop-button"),
			schedule					: document.getElementById("task-schedule-button")
		};

		this.taskconfigBar 				= document.getElementById("taskconfig-sidebar");
		this.taskconfig					= document.getElementById("taskconfig");
		this.taskconfigHeading			= this.taskconfig.getElementsByClassName("heading")[0];
		this.taskSettings				= document.getElementById("task-settings");
		this.taskProperties 			= Array.from(this.taskSettings.querySelectorAll(".task-property")).reduce((acc, cur) => {
			acc[cur.dataset.name]		= cur.querySelector(".task-input");
			return acc;
		}, {});

		this.loadingScreen				= document.getElementById("loading-screen");
	}   //@formatter:on

	createElements() {
		// Create state nodes in the timeline
		const states = ["stop", "flush", "sample", "clean", "preserve", "decontaminate"];
		this.stateNodes = states.map((stateName, index) => {
			const node = document.createElement("button");
			node.classList.add("state-node");
			node.style.background = "white";
			node.style.color = stateColors[index % stateColors.length];
			node.textContent = stateName.toUpperCase();
			node.dataset.name = stateName;
			this.stateTimeline.appendChild(node);
			return node;
		});

		// Create valve overview buttons
		this.valves = [...Array(24).keys()].map(i => {
			const valve = document.createElement("button");
			valve.textContent = `${i}`;
			valve.onclick = () => {valve.classList.toggle("selected");};
			valve.disabled = true;
			return valve;
		});

		// Append to the overview in reverse order for the first half of the valves
		for (let i = 0; i < this.valves.length; i++) {
			this.valveOverview.append(this.valves[i < 12 ? 11 - i: i]);
		}

		// Apply styling to state configs
		this.stateConfigs.forEach(config => {
			const index = states.findIndex(name => config.dataset.name === name);
			config.getElementsByClassName("heading")[0].style.background = stateColors[index % stateColors.length];
		});
	}

	updateStatus(data) {
		const {stateName = "undefined", stateId = NaN, valveCurrent = NaN, valveUpperBound = NaN, pressure = NaN, temperature = NaN, waterFlow = NaN} = data;
		const {valves} = data;
		const tags = this.statusTags;
		//@formatter:off
		tags["currentState"].textContent 	= stateName.toUpperCase();
		tags["currentValve"].textContent 	= valveCurrent;
		tags["totalValves"].textContent 	= valveUpperBound + 1;
		tags["pressure"].textContent	 	= `${pressure.toFixed(2)} PSI`;
		tags["temperature"].textContent 	= `${temperature.toFixed(2)} °C`;
		tags["waterFlow"].textContent 		= `${waterFlow.toFixed(2)} mL/s`;
		tags["time-local"].textContent 		= new Date(data["timeUTC"] * 1000).toString();
		//@formatter:on
		valves.forEach((i, index) => (this.valves[index].disabled = (i === 0)));
	}

	updateTaskChooser(taskrefs) {
		// Remove all tasks
		for (let i = this.tasks.length - 1; i >= 0; i--) {
			const node = this.tasks.item(i);
			node.remove();
		}

		// Replace with new tasks
		taskrefs.map(ref => {
			const name = document.createElement("i");
			name.className = "task-name";
			name.textContent = ref["name"];

			const status = document.createElement("span");
			status.className = "task-status";
			status.textContent = ref["status"];

			const task = document.createElement("li");
			task.className = "task";
			task.append(name);
			task.append(status);
			task.dataset.id = ref["id"];
			this.taskListings.append(task);
		});
	}

	updateTaskSettings(task) {
		const {name, date, time, valvesAssigned, valveInterval, notes} = this.taskProperties;
		const datetime      = new Date(task["schedule"]),
			  year          = datetime.getFullYear(),
			  month         = `${datetime.getMonth() + 1}`.padStart(2, "0"),
			  dayOfTheMonth = `${datetime.getDate()}`.padStart(2, "0"),
			  hour          = `${datetime.getHours()}`.padStart(2, "0"),
			  minute        = `${datetime.getMinutes()}`.padStart(2, "0");

		name.value = task["name"];
		date.value = `${year}-${month}-${dayOfTheMonth}`;
		time.value = `${hour}:${minute}`;
		valvesAssigned.value = task["valves"].join(",");
		valveInterval.value = task["interval"];
		notes.textContent = task["notes"];
	}
}


//===============================================================
// [+_+] Controller
//===============================================================
class Controller {
	load() {
		// Load the view and setup status button onclick
		view.statusButton.onclick = () => {
			this.statusButtonClicked();
		};

		// State Nodes
		const stateRequest = (name) => {
			this.showLoadingScreen();
			fetch(host + "select-state", {
				method: "POST", body: JSON.stringify({"name": name})
			}).catch(error => {
				console.log(error);
			}).finally(() => {
				this.hideLoadingScreen();
			})
		};

		view.stateNodes.forEach(node => {
			node.onclick = () => {stateRequest(node.textContent.trim().toLowerCase())}
		});

		// Configure onclick event for each header button
		Object.keys(view.headerButtons).forEach(key => {
			const button = view.headerButtons[key];
			button.addEventListener("click", () => {this.selectHeaderButton(key)})
		});

		view.headerButtons.rtc.onclick = () => {
			this.showLoadingScreen();
			fetch(host + "setTime", {
				method: "POST",
				body: JSON.stringify({
					timeUTC: Math.trunc(Date.now() / 1000)
				})
			}).catch(error => {
				console.log(error);
			}).finally(() => {
				this.hideLoadingScreen();
			});
		};

		// Request function for task API
		const taskRequest = (path) => {
			const selected = Array.from(view.tasks).filter(task => task.classList.contains("selected"))[0];
			if (!selected) {
				console.log("No task selected");
				return;
			}

			if (online) {
				this.showLoadingScreen();
			}
			fetch(host + path, {
				method: "POST", body: JSON.stringify({id: selected.dataset.id})
			}).then(response => response.json()).then(task => {
				view.updateTaskSettings(task);
			}).finally(() => {
				this.hideLoadingScreen();
			});
		};

		//@formatter:off
		view.taskButtons.view.onclick 		= () => taskRequest("getTask");
		view.taskButtons.stop.onclick 		= () => taskRequest("stopTask");
		view.taskButtons.schedule.onclick 	= () => taskRequest("scheduleTask");
		//@formatter:on
	}

	statusButtonClicked() {
		view.statusButton.classList.toggle("selected");
		const statusButtonSelected = view.statusButton.classList.contains("selected");
		view.statusBar.classList.toggle("reveal", statusButtonSelected);
		document.querySelector("body .space").classList.toggle("reveal", statusButtonSelected);
	}

	selectTask(index) {
		const task = view.tasks[index];
		Array.from(view.tasks).forEach(t => {t.classList.remove("selected");});
		view.taskconfigHeading.value = task.textContent;
		task.classList.add("selected");
	}

	selectHeaderButton(name) {
		const button = view.headerButtons[name];
		if (button === view.headerButtons.task) {
			button.classList.toggle("selected");
			const taskButtonSelected = button.classList.contains("selected");
			view.main.classList.toggle("reveal", taskButtonSelected);
			view.taskconfigBar.classList.toggle("reveal", taskButtonSelected);
			document.querySelector("main .space").classList.toggle("reveal", taskButtonSelected);
			return;
		}

		Object.keys(view.headerButtons).forEach(key => {
			if (view.headerButtons[key] === view.headerButtons.task) {
				return;
			}

			if (view.headerButtons[key] !== button) {
				view.headerButtons[key].classList.remove("selected");
			}
		});
	}

	fetchTaskrefsFromServer() {
		withTimeout(3000, fetch(host + "taskrefs")).then(response => response.json()).then(taskrefs => {
			view.updateTaskChooser(taskrefs);
			Array.from(view.tasks).forEach((task, i) => {
				task.onclick = () => {this.selectTask(i);};
			});
		}).catch(error => console.log(error));
	}

	showLoadingScreen() {
		view.loadingScreen.classList.remove("hide");
		disableScroll();
	}

	hideLoadingScreen() {
		view.loadingScreen.classList.add("hide");
		enableScroll();
	}
}


window.onload = function () {
	view = new View();
	view.createElements();

	controller = new Controller();
	controller.load();

	controller.selectTask(0);
	controller.selectHeaderButton("task");
	controller.statusButtonClicked();
	controller.fetchTaskrefsFromServer();

	status = new Status();
	status.beginLiveStatusUpdate();
	// status.stopLiveStatusUpdate();
};