#include "OPSystem.hpp"
#include "OPServer.hpp"
#include "OPShiftRegister.hpp"
#include "OPPump.hpp"
#include "OPPowerManagement.hpp"
#include "SSC.h"
#include "ArduinoJson.h"

// Water 	0, 0
// Air 		0, 1
// Alcohol 	0, 2
// Flush 	0, 3 

OPServer web("eDNA WebServer", "eDNA", "password");
OPShiftRegister shift("TPIC Shift Registers", 32);
OPPump pump("Main Peristaltic Pump", 12, 13);
OPPower power("Power Management Controlled using DS3231 as a time keeper");
SSC pressureSensor(0x08);

enum class State {flush, sample, clean, preserve, stop, standBy};
Status status;

void idle_state() {}

void stop_state() {
	if(app.isInitialTransition()) {
		pump.off();
	}

	if (app.timeSinceLastTransition() >= 2000) {
		shift.setZeros();
		shift.flush();

		if (status.currentValve >= status.endValve) {
			status.currentValve = status.endValve + 1;
			status.isFull = true;
		} else {
			status.currentValve++;
			power.scheduleNextAlarm(status.samplingInteval);
		}

		app.transitionTo(State::standBy);
		Serial.println("Transitioning to Idle");
	}
}

void preserve_state() {
	if (app.isInitialTransition()) {
		int r = shift.registerNumber(status.currentValve) + 1;
		int b = shift.bitNumber(status.currentValve);

		shift.setZeros();				// Reset TPIC Values
		shift.setRegister(0, 2, HIGH);	// Alcohol Intake
		shift.setRegister(r, b, HIGH); 	// Filter
		shift.flush();
		pump.on();
	}
	
	// Transitioning Condition
	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::stop);
		Serial.println("Transitioning to Stop");
	}
}

void clean_state() {
	if (app.isInitialTransition()) {
		int r = shift.registerNumber(status.currentValve) + 1;
		int b = shift.bitNumber(status.currentValve);

		shift.setZeros();				// Reset TPIC Values
		shift.setRegister(0, 1, HIGH); 	// Air Intake
		shift.setRegister(r, b, HIGH); 	// Filter
		shift.flush();
		pump.on();
	}

	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::preserve);
		Serial.println("Transitioning to Preserve");
	}
}

void sample_state() {
	if (app.isInitialTransition()) {
		int r = shift.registerNumber(status.currentValve) + 1;
		int b = shift.bitNumber(status.currentValve);

		shift.setZeros();				// Reset TPIC Values
		shift.setRegister(0, 0, HIGH); 	// Water Intake
		shift.setRegister(r, b, HIGH); 	// Filter
		shift.flush();
		pump.on();

		Serial.println(status.currentValve + 1);
	}

	// Transition to clean when the pressure exceeds 8 PSI
	// if (pressureSensor.pressure() >= 8) {
	// 	app.transitionTo(State::clean);
	// 	Serial.println("Transitioning to Clean");
	// }
	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::clean);
		Serial.println("Transitioning to Clean");
	}
}

void flush_state() {	
	if (app.isInitialTransition()) {
		shift.setZeros();				// Reset TPIC Values
		shift.setRegister(0, 0, HIGH); 	// Water Intake
		shift.setRegister(0, 3, HIGH); 	// Flush Valve
		shift.flush();
		pump.on();
	}

	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::sample);
		Serial.println("Transitioning to Sampling");
	}
}

void prepare() {
	// <==> Serial <==>
	Serial.begin(9600); delay(5000);

	// <==> States <==>
	app.registerState(State::flush, "flush", flush_state);
	app.registerState(State::sample, "sample", sample_state);
    app.registerState(State::clean, "clean", clean_state);
    app.registerState(State::preserve, "preserve", preserve_state);
    app.registerState(State::stop, "stop", stop_state);
    app.registerState(State::standBy, "standBy", idle_state);
	
	// <==> Power <==>
	addComponent(&power);

	// <==> Web: HTTP Routering <==>
	web.on("/", [](Request & req, Response & res) {
		const char index[] = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='UTF-8'><title>eDNA Web Interface</title><style>*{padding:0;margin:0}body{font-family:verdana;font-size:12px;box-sizing:border-box;width:100vw;height:100vh}body{display:grid;grid-template-columns:256px 1fr;grid-template-rows:96px 1fr;grid-template-areas:'logo header' ' status main'}#logo{display:flex;flex-direction:column;justify-content:center;text-align:center;background:rgb(55, 100, 200);grid-area:logo;font:20px verdana;font-weight:bold;color:white;z-index:2;box-shadow:0px 0px 8px 0px #888}header{background:rgb(130, 155, 195);grid-area:header;box-shadow:0px 0px 8px 0px #888;z-index:1}nav{background:rgb(130,155,195);grid-area:nav}aside{display:flex;flex-direction:column;background:rgb(240,240,240);grid-area:status}main{display:grid;justify-content:center;align-content:center;grid-gap:24px;background:white;grid-area:main;box-shadow:0px 0px 4px 0px #ddd}.status-widget,.status-widget-fix{background:white;height:54px;margin:12px;margin-top:0;margin-bottom:12px;border-radius:4px;box-shadow:0px 4px 4px 0px #ddd}.status-widget-fix{display:grid;grid-template-columns:1fr 1fr;height:80px;margin:0}.status-widget-fix .status-tag{display:grid;justify-content:center}.status-widget-fix .key{align-self:flex-end;padding:4px 0 4px 0}.key{color:#888}.value{font-weight:bold}.status-widget{display:grid;align-items:center}.status-widget .status-tag{display:grid;grid-template-columns:1fr 1fr;text-align:center}.status-divider{background:rgb(240, 240, 240);padding:24px;text-align:center;font-size:12px;color:#888}header p{margin:72px 12px;text-align:right;color:white}.btn{text-align:center;padding:24px 32px;background:red;font-size:16px;color:white;border-radius:999px;filter:opacity(50%)}.btn-selected{filter:opacity(100%)}.btn:hover{cursor:pointer;filter:opacity(100%)}.btn:active{filter:brightness(125%)}</style> <script>function selectButton(index){const btns=document.getElementsByClassName('btn');if(isNaN(index)||index>=btns.length){return;} for(var i=0;i<btns.length;i++){btns[i].classList.remove('btn-selected');} btns[index].classList.add('btn-selected');} function readStatus(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText){var res=JSON.parse(this.responseText);document.getElementById('s').textContent=res[\"state-name\"].toUpperCase();document.getElementById('p').textContent=res[\"pressure\"];document.getElementById('t').textContent=res[\"temperature\"];document.getElementById('cv').textContent=parseInt(res[\"valve-current\"])+1;document.getElementById('tv').textContent=parseInt(res[\"valve-max\"])+1;document.getElementById('i').textContent=res[\"routine-interval\"];selectButton(res[\"state-id\"]);}};request.open('GET','status',true);request.send(null);setTimeout(readStatus,1000);} function flush(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','flush',true);request.send(null);} function sample(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','sample',true);request.send(null);} function clean(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','clean',true);request.send(null);} function preserve(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','preserve',true);request.send(null);} function stop(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','stop',true);request.send(null);} var i=0;function nextButton(){selectButton(i++%document.getElementsByClassName('btn').length);setTimeout(nextButton,1000);} window.onload=function(){const colors=['#173F5F','#20639B','#3CAEA3','#ED553B'];const btns=document.getElementsByClassName('btn');console.log(btns);for(let i=0;i<btns.length;i++){btns[i].style.background=colors[i%colors.length];} setTimeout(readStatus,1000);}</script> </head><body><div id='logo'><p>DASHBOARD</p></div><header><p>eDNA Web Interface (0.0.2-alpha)</p> </header> <aside id='status-bar'><div class='status-widget-fix'><div class='status-tag'><p class='key'>ID Number</p><p class='value'>#00001</p></div><div class='status-tag'><p class='key'>Connection</p><p class='value'>OK</p></div></div><div class='status-divider'>CURRENT STATUS</div><div class='status-widget'><div class='status-tag'><p class='key'>Current State</p><p class='value' id='s'>Stand By</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Current Valve</p><p class='value' id='cv'>0</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Total Valve</p><p class='value' id='tv'>0</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Interval</p><p class='value'><span id='i'>0</span>S</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Pressure</p><p class='value'><span id='p'>0</span> PSI</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Temperature</p><p class='value'><span id='t'>0</span> °C</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Volume</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Depth</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Flow Speed</p><p class='value'>---</p></div></div></aside><main><div class='btn' onclick='flush()'> FLUSH</div><div class='btn' onclick='sample()'> SAMPLE</div><div class='btn' onclick='clean()'> CLEAN</div><div class='btn' onclick='preserve()'> PRESERVE</div><div class='btn' onclick='stop()'> STOP</div> </main></body></html>";
        res.send(index);
        res.end();
    });

	web.on("/status", [](Request & req, Response & res) {
		pressureSensor.update();
		res.contentType = "application/json";

		const size_t size = JSON_OBJECT_SIZE(11);
		StaticJsonDocument<size> doc;
		doc["state-name"] = app.getCurrentStateName();
		doc["state-id"] = static_cast<int>(app.getCurrentState());
		doc["valve-begin"] = status.startValve;
		doc["valve-current"] = status.currentValve;
		doc["valve-end"] = status.endValve;
		doc["valve-max"] = status.maxValve;
		doc["pressure"] = pressureSensor.pressure();
		doc["temperature"] = pressureSensor.temperature();
		doc["routine-interval"] = status.samplingInteval;
		char output[size];
		serializeJson(doc, output);
		res.send(output);
		res.end();

		// Serial.println(FreeRam());
	});

	web.on("/flush", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::flush);
		Serial.println("Transitioning to Flushing State");
	});

	web.on("/sample", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::sample);
		Serial.println("Transitioning to Sampling State");
	});

	web.on("/clean", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::clean);
		Serial.println("Transitioning to Cleaning State");
	});

	web.on("/preserve", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::preserve);
		Serial.println("Transitioning to Preserving State");
	});

	web.on("/stop", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::stop);
		Serial.println("Transitioning to Stop State");
	});
	addComponent(&web);

	// <==> Shift Registers <==>
	shift.setPins(5, 9, 11); // 5 9 11
	addComponent(&shift);

	// <==> Pressure Sensor <==>
	Wire.begin();
	pressureSensor.setMinRaw(1638);		// Please refer to datasheet
  	pressureSensor.setMaxRaw(14745);	// 		
  	pressureSensor.setMinPressure(0);	//
  	pressureSensor.setMaxPressure(30);	//
	pressureSensor.start();				//

	// <==> Transition to Initial State <==>
	app.transitionTo(State::standBy);
}

void ready() {
	setTimeout(1000, []() {
		web.printWiFiStatus();
		web.begin();
	});

	// rtc.setAlarm(ALM1_MATCH_SECONDS, 0, 0, 0);

	// if (status.isProgrammingMode() == false) {
	// 	app.transitionTo(State::flush);
	// }
}

void update() {
	if (alarmTriggered && app.getCurrentState() == State::standBy) {
		alarmTriggered = false;
		Serial.println("Alarm Triggered");
		app.transitionTo(State::flush);
	}
}

OPSystem app(prepare, ready, update);

// shift.setPins(5, 9, 10); // For LED TPICs 
// shift.setPins(23, 24, 11);
// shift.setPins(23, 24, 10);


