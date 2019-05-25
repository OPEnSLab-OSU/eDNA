#include "OPSystem.hpp"
#include "OPServer.hpp"
#include "OPShiftRegister.hpp"
#include "OPPump.hpp"
#include "SSC.h"

OPServer webserver("eDNA WebServer", "eDNA", "password");
OPShiftRegister shift("TPIC Shift Registers", 32);
OPPump pump("Main Peristaltic Pump", 12, 13);
SSC pressureSensor(0x08);

enum class State {flushing, sampling, clean, preserve, stop, idle};

void idle_state() {}

void stop_state() {
	pump.off();
	if (app.timeSinceLastTransition() >= 1000) {
		shift.setZeros();
		shift.flush();
		app.transitionTo(State::idle);
		Serial.println("Transitioning to Idle");
	}
}

void preserve_state() {
	shift.setZeros();			// Reset TPIC Values
	shift.setRegister(0, 4);	// Alcohol
	shift.setRegister(2, 16); 	// Filter
	shift.flush();
	pump.on();

	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::stop);
		Serial.println("Transitioning to Stop");
	}
}

void clean_state() {
	shift.setZeros();			// Reset TPIC Values
    shift.setRegister(0, 2); 	// Air Intake
	shift.setRegister(2, 16); 	// Filter
	shift.flush();
	pump.on();

	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::preserve);
		Serial.println("Transitioning to Preserve");
	}
}

void sampling_state() {
	shift.setZeros();			// Reset TPIC Values
    shift.setRegister(0, 1); 	// Water Intake
	shift.setRegister(2, 16); 	// Filter
	shift.flush();
	pump.on();

	// Transition to clean when the pressure exceeds 12 PSI
	if (pressureSensor.pressure() >= 8) {
		app.transitionTo(State::clean);
		Serial.println("Transitioning to Clean");
	}
}

void flushing_state() {			
	shift.setZeros();			// Reset TPIC Values
	shift.setRegister(0, 9);	// Flush Valve + Water Intake
	shift.flush();
	pump.on();

	if (app.timeSinceLastTransition() >= 10000) {
		app.transitionTo(State::sampling);
		Serial.println("Transitioning to Sampling");
	}
}

void prepare() {
	// <==> Serial <==>
	Serial.begin(9600); delay(1000);

	// <==> States <==>
	app.registerState(State::flushing, flushing_state);
	app.registerState(State::sampling, sampling_state);
    app.registerState(State::clean, clean_state);
    app.registerState(State::preserve, preserve_state);
    app.registerState(State::stop, stop_state);
    app.registerState(State::idle, idle_state);

	// <==> Web: HTTP Routering <==>
	webserver.on("/", [](Request & req, Response & res) {
		const char index[] = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='UTF-8'><title>eDNA Web Interface</title><style>*{padding:0;margin:0}body{font-family:verdana;font-size:12px;box-sizing:border-box;width:100vw;height:100vh}body{display:grid;grid-template-columns:256px 1fr;grid-template-rows:96px 1fr;grid-template-areas:'logo header' ' status main'}#logo{display:flex;flex-direction:column;justify-content:center;text-align:center;background:rgb(55, 100, 200);grid-area:logo;font:20px verdana;font-weight:bold;color:white;z-index:2;box-shadow:0px 0px 8px 0px #888}header{background:rgb(130, 155, 195);grid-area:header;box-shadow:0px 0px 8px 0px #888;z-index:1}nav{background:rgb(130,155,195);grid-area:nav}aside{display:flex;flex-direction:column;background:rgb(240,240,240);grid-area:status}main{display:grid;justify-content:center;align-content:center;grid-gap:24px;background:white;grid-area:main;box-shadow:0px 0px 4px 0px #ddd}.status-widget,.status-widget-fix{background:white;height:54px;margin:12px;margin-top:0;margin-bottom:12px;border-radius:4px;box-shadow:0px 4px 4px 0px #ddd}.status-widget-fix{display:grid;grid-template-columns:1fr 1fr;height:80px;margin:0}.status-widget-fix .status-tag{display:grid;justify-content:center}.status-widget-fix .key{align-self:flex-end;padding:4px 0 4px 0}.key{color:#888}.value{font-weight:bold}.status-widget{display:grid;align-items:center}.status-widget .status-tag{display:grid;grid-template-columns:1fr 1fr;text-align:center}.status-divider{background:rgb(240, 240, 240);padding:24px;text-align:center;font-size:12px;color:#888}header p{margin:72px 12px;text-align:right;color:white}.btn{padding:16px 32px;background:red;font-size:16px;color:white;text-align:center;border-radius:999px}.btn:hover{cursor:pointer;filter:brightness(85%)}.btn:active{filter:brightness(115%)}</style> <script>function readStatus(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText){const values=this.responseText.split(':');document.getElementById('s').textContent=values[0].trim();document.getElementById('p').textContent=values[1].trim();document.getElementById('t').textContent=values[2].trim();}};request.open('GET','status',true);request.send(null);setTimeout(readStatus,1000);} function flush(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','flush',true);request.send(null);} function sample(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','sample',true);request.send(null);} function clean(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','clean',true);request.send(null);} function preserve(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','preserve',true);request.send(null);} function stop(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','stop',true);request.send(null);} window.onload=function(){const colors=['#173F5F','#20639B','#3CAEA3','#ED553B'];const btns=document.getElementsByClassName('btn');console.log(btns);for(let i=0;i<btns.length;i++){btns[i].style.background=colors[i%colors.length];} setTimeout(readStatus,1000);}</script> </head><body><div id='logo'><p>DASHBOARD</p></div><header><p>eDNA Web Interface (0.0.1-alpha)</p></header><aside id='status-bar'><div class='status-widget-fix'><div class='status-tag'><p class='key'>ID Number</p><p class='value'>#32123</p></div><div class='status-tag'><p class='key'>Connection</p><p class='value'>OK</p></div></div><div class='status-divider'>CURRENT STATUS</div><div class='status-widget'><div class='status-tag'><p class='key'>Current State</p><p class='value' id='s'>Stand By</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Pressure</p><p class='value'><span id='p'>0</span> PSI</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Temperature</p><p class='value'><span id='t'>0</span> °C</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Volume</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Depth</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Flow Speed</p><p class='value'>---</p></div></div></aside><main><div class='btn' onclick='flush()'> FLUSH</div><div class='btn' onclick='sample()'> SAMPLE</div><div class='btn' onclick='clean()'> CLEAN</div><div class='btn' onclick='preserve()'> PRESERVE</div><div class='btn' onclick='stop()'> STOP</div> </main></body></html>";
        res.send(index);
        res.end();
    });

    webserver.on("/status", [](Request & req, Response & res) {
        res.contentType = "text/plain";
		
		switch (app.getCurrentState()) {
			case State::flushing:
				res.send("Flushing");
				break;
			case State::sampling:
			 	res.send("Sampling");
				break;
			case State::clean:
			 	res.send("Cleaning");
				break;
			case State::preserve:
				res.send("Preserving");
				break;
			case State::stop:
			 	res.send("Stop"); 
				break;
			default:
				res.send("Stand By"); 	
				break;
		}
		
		res.send(":");
		res.send(static_cast<int>(app.getCurrentState()));
		res.send(":");
		pressureSensor.update();
		res.send(pressureSensor.pressure());
		res.send(":");
		res.send(pressureSensor.temperature());
        res.end();
    });

	// <==> Web: State HTTP Handling <==>
	webserver.on("/flush", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::flushing);
		Serial.println("Transitioning to Flushing State");
	});

	webserver.on("/sample", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::sampling);
		Serial.println("Transitioning to Sampling State");
	});

	webserver.on("/clean", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::clean);
		Serial.println("Transitioning to Cleaning State");
	});

	webserver.on("/preserve", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::preserve);
		Serial.println("Transitioning to Preserving State");
	});

	webserver.on("/stop", [](Request & req, Response & res) {
		res.contentType = "text/plain";
		res.send("success"); res.end();
		app.transitionTo(State::stop);
		Serial.println("Transitioning to Stop State");
	});
	addComponent(&webserver);

	// <==> Shift Registers <==>
	shift.setPins(5, 9, 11);
	addComponent(&shift);

	// <==> Pressure Sensor <==>
	Wire.begin();
	pressureSensor.setMinRaw(1638);
  	pressureSensor.setMaxRaw(14745);
  	pressureSensor.setMinPressure(0);
  	pressureSensor.setMaxPressure(30);
	pressureSensor.start();

	// <==> Transition to Initial State <==>
	app.transitionTo(State::idle);
}

void ready() {
	setTimeout(1000, []() {
		webserver.printWiFiStatus();
		webserver.begin();
	});
}

void update() {

}

OPSystem app(prepare, ready, update);

// shift.setPins(5, 9, 10); // For LED TPICs 
// shift.setPins(23, 24, 11);
// shift.setPins(23, 24, 10);

/* JSON Object Describe the Current State of eDNA Machine
{
	"state": string,
	"pressure": number,
	"temperature": number,
	"valve": number,
	"max-valve": number
}
*/

