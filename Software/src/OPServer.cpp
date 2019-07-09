#include "OPServer.hpp"

// =================================================
// MARK: Response Implementation
// =================================================
void Response::sendHeader() {
    if (isHeaderSent) {
        return;
    } else {
        isHeaderSent = true;
    }

    // HTTP Status
    client.print(F("HTTP/1.1 "));
    switch (status) {
    case 200:
        client.println(F("200 OK"));
        break;
    case 302:
        client.println(F("302 Redirect"));
        break;
    default:
        client.println(F("404 Not Found"));
        break;
    }

    // Content-Type
    client.print(F("Content-Type: "));
    client.println(contentType);

    // Connection
    client.println(F("Connection: close"));

	// End of header
    client.println();
}

void Response::send(const char s[]) {
    sendHeader();
    const int size = 768;
    for (unsigned int i = 0; i < strlen(s); i += size) {
        char part[size + 1] = {0};
        strncpy(part, s + i, size);
        client.print(part);
    }
}

void Response::send(Printable & p) {
    sendHeader();
    client.print(p);
}

void Response::send(int i) {
    sendHeader();
    client.print(i);
}

void Response::send(float f) {
    sendHeader();
    client.print(f);
}

void Response::sendNewline() {
	sendHeader();
	client.println();
}

void Response::end() {
    sendHeader();
    delay(5);
    client.stop();
}

// =================================================
// MARK: OPServer Implementation
// =================================================
void OPServer::begin() {
    isRunning = true;
}

void OPServer::setup() {
    WiFi.setPins(8, 7, 4, 2);
    WiFi.beginAP(ssid, pass);
    while ((status = WiFi.status()) != WL_AP_LISTENING) {
        WiFi.beginAP(ssid, pass);
        Serial.println(F("WiFi AP Mode Failed to Initialize"));
        Serial.println(F("Try again in "));
        for (char i = 3; i > 0; i++) {
            Serial.println(i);
            delay(1000);
        }
    }

	// homepage = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='UTF-8'><title>eDNA Web Interface</title><style>*{padding:0;margin:0}body{font-family:verdana;font-size:12px;box-sizing:border-box;width:100vw;height:100vh}body{display:grid;grid-template-columns:256px 1fr;grid-template-rows:96px 1fr;grid-template-areas:'logo header' ' status main'}#logo{display:flex;flex-direction:column;justify-content:center;text-align:center;background:rgb(55, 100, 200);grid-area:logo;font:20px verdana;font-weight:bold;color:white;z-index:2;box-shadow:0px 0px 8px 0px #888}header{background:rgb(130, 155, 195);grid-area:header;box-shadow:0px 0px 8px 0px #888;z-index:1}nav{background:rgb(130,155,195);grid-area:nav}aside{display:flex;flex-direction:column;background:rgb(240,240,240);grid-area:status}main{display:grid;justify-content:center;align-content:center;grid-gap:24px;background:white;grid-area:main;box-shadow:0px 0px 4px 0px #ddd}.status-widget,.status-widget-fix{background:white;height:54px;margin:12px;margin-top:0;margin-bottom:12px;border-radius:4px;box-shadow:0px 4px 4px 0px #ddd}.status-widget-fix{display:grid;grid-template-columns:1fr 1fr;height:80px;margin:0}.status-widget-fix .status-tag{display:grid;justify-content:center}.status-widget-fix .key{align-self:flex-end;padding:4px 0 4px 0}.key{color:#888}.value{font-weight:bold}.status-widget{display:grid;align-items:center}.status-widget .status-tag{display:grid;grid-template-columns:1fr 1fr;text-align:center}.status-divider{background:rgb(240, 240, 240);padding:24px;text-align:center;font-size:12px;color:#888}header p{margin:72px 12px;text-align:right;color:white}.btn{text-align:center;padding:24px 32px;background:red;font-size:16px;color:white;border-radius:999px;filter:opacity(50%)}.btn-selected{filter:opacity(100%)}.btn:hover{cursor:pointer;filter:opacity(100%)}.btn:active{filter:brightness(125%)}</style> <script>function selectButton(index){const btns=document.getElementsByClassName('btn');if(isNaN(index)||index>=btns.length){return;} for(var i=0;i<btns.length;i++){btns[i].classList.remove('btn-selected');} btns[index].classList.add('btn-selected');} function readStatus(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText){var res=JSON.parse(this.responseText);document.getElementById('s').textContent=res[\"state-name\"].toUpperCase();document.getElementById('p').textContent=res[\"pressure\"];document.getElementById('t').textContent=res[\"temperature\"];document.getElementById('cv').textContent=parseInt(res[\"valve-current\"])+1;document.getElementById('tv').textContent=parseInt(res[\"valve-max\"])+1;document.getElementById('i').textContent=res[\"routine-interval\"];selectButton(res[\"state-id\"]);}};request.open('GET','status',true);request.send(null);setTimeout(readStatus,1000);} function flush(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','flush',true);request.send(null);} function sample(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','sample',true);request.send(null);} function clean(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','clean',true);request.send(null);} function preserve(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','preserve',true);request.send(null);} function stop(){const request=new XMLHttpRequest();request.onreadystatechange=function(){if(this.readyState!==4||this.status!==200){return;} if(this.responseText==='success'){}};request.open('GET','stop',true);request.send(null);} var i=0;function nextButton(){selectButton(i++%document.getElementsByClassName('btn').length);setTimeout(nextButton,1000);} window.onload=function(){const colors=['#173F5F','#20639B','#3CAEA3','#ED553B'];const btns=document.getElementsByClassName('btn');console.log(btns);for(let i=0;i<btns.length;i++){btns[i].style.background=colors[i%colors.length];} setTimeout(readStatus,1000);}</script> </head><body><div id='logo'><p>DASHBOARD</p></div><header><p>eDNA Web Interface (0.0.2-alpha)</p> </header> <aside id='status-bar'><div class='status-widget-fix'><div class='status-tag'><p class='key'>ID Number</p><p class='value'>#00001</p></div><div class='status-tag'><p class='key'>Connection</p><p class='value'>OK</p></div></div><div class='status-divider'>CURRENT STATUS</div><div class='status-widget'><div class='status-tag'><p class='key'>Current StateIdentifier</p><p class='value' id='s'>Stand By</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Current Valve</p><p class='value' id='cv'>0</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Total Valve</p><p class='value' id='tv'>0</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Interval</p><p class='value'><span id='i'>0</span>S</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Pressure</p><p class='value'><span id='p'>0</span> PSI</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Temperature</p><p class='value'><span id='t'>0</span> °C</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Volume</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Water Depth</p><p class='value'>---</p></div></div><div class='status-widget'><div class='status-tag'><p class='key'>Flow Speed</p><p class='value'>---</p></div></div></aside><main><div class='btn' onclick='flush()'> FLUSH</div><div class='btn' onclick='sample()'> SAMPLE</div><div class='btn' onclick='clean()'> CLEAN</div><div class='btn' onclick='preserve()'> PRESERVE</div><div class='btn' onclick='stop()'> STOP</div> </main></body></html>";
}

void OPServer::update() {
    if (isRunning == false) {
        return;
    }

    // WiFi status has changed
    if (status != WiFi.status()) {
        if ((status = WiFi.status()) == WL_AP_CONNECTED) {
            byte remoteMac[6];
            WiFi.APClientMacAddress(remoteMac);
            Serial.println(F("Device connected to AP"));
			Serial.print(F("MAC Address: "));
            printMacAddress(remoteMac);
            server.begin();
        } else {
            Serial.println(F("Device disconnected from AP"));
        }
    }

    if (status != WL_AP_CONNECTED) {
        return;
    }

    WiFiClient client = server.available();
    if (client && client.connected() && client.available()) {
        const int size = 768;
        char httpRequest[size + 1];

		// HTTP Request Header 
		// NOTE: Blank line is at the end of request header
		delay(10);
		int headerLength = client.available();
		Serial.print("Header-Length: ");
		Serial.println(headerLength);
		client.readBytes(httpRequest, headerLength);

		// HTTP Request Body
		// NOTE: This is needed to ensure that request body is received
		delay(10);
		int contentLength = client.available();
		if (contentLength > 0) {
			Serial.print("Content-Length: ");
			Serial.println(contentLength);
			client.readBytes(httpRequest + headerLength, contentLength);
		} 

		httpRequest[headerLength + contentLength] = 0;
        Request request(httpRequest, client);
        handleRequest(request);
    }
}

void OPServer::on(const char * path, const char * method, RequestFunctionPointer callback) {
    handlers[size].path = path;
	handlers[size].method = method;
    handlers[size].callback = callback;
    size++;
}

void OPServer::get(const char * path, RequestFunctionPointer callback) {
	OPServer::on(path, "GET", callback);
}

void OPServer::post(const char * path, RequestFunctionPointer callback) {
	OPServer::on(path, "POST", callback);
}

void OPServer::handleRequest(Request & req) {
    Response res(req.client);
    for (int i = 0; i < size; i++) {
        if (strcmp(handlers[i].path, req.path) == 0 && strcmp(handlers[i].method, req.method) == 0) {
            handlers[i].callback(req, res);
            return;
        }
    }

    WiFiClient & client = req.client;
    client.println(F("HTTP/1.1 404 Not Found"));
    client.println(F("Connection: close"));
    client.println();
    client.stop();
}

void OPServer::printWiFiStatus() {
    // SSID:
    Serial.print(F("SSID: "));
    Serial.println(WiFi.SSID());

    // IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print(F("IP Address: "));
    Serial.println(ip);

    // Signal strength:
    long rssi = WiFi.RSSI();
    Serial.print(F("Signal strength (RSSI):"));
    Serial.print(rssi);
    Serial.println(F(" dBm"));

    // Address:
    Serial.print(F("Web Browser: http://"));
    Serial.println(ip);
}

void OPServer::printMacAddress(byte mac[]) {
    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0) {
            Serial.print(":");
        }
    }
    Serial.println();
}