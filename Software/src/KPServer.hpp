#pragma once
#include "KPComponent.hpp"
#include "KPFoundation.hpp"
#include "KPServerRequest.hpp"
#include "KPServerResponse.hpp"
#include "SPI.h"
#include "WiFi101.h"
// #include "KPSDCard.hpp"

using RequestFunctionPointer = void (*)(Request & req, Response & res);

struct RequestHandler {
    const char * path;
    const char * method;
    RequestFunctionPointer callback;
};

class KPServer : public KPComponent {
public:
    const char * ssid;
    const char * pass;

    uint8_t status = WL_IDLE_STATUS;
    bool isRunning = false;

    Array<RequestHandler> handlers{10};
    WiFiServer server;

    KPServer(const char * name, const char * ssid, const char * pass)
        : KPComponent(name), ssid(ssid), pass(pass), server(80) {}

    void setup() {
        WiFi.setPins(8, 7, 4, 2);
		WiFi.lowPowerMode();
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
    }

	void begin() {
    	isRunning = true;
	}

    void update() {
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

		// Short circuit if client is not connected
        if (status != WL_AP_CONNECTED) {
            return;
        }

        WiFiClient client = server.available();
        if (client && client.connected() && client.available()) {
            const int size = 1024;
            char httpRequest[size + 1]{0};

            // HTTP Request Header
            // NOTE: Blank line is at the end of request header
            delay(10);
            int headerLength = client.available();
            // Serial.print("Header-Length: ");
            // Serial.println(headerLength);
            client.readBytes(httpRequest, headerLength);

            // HTTP Request Body
            // NOTE: This is needed to ensure that request body is received
            delay(10);
            int contentLength = client.available();
            if (contentLength > 0) {
                // Serial.print("Content-Length: ");
                // Serial.println(contentLength);
                client.readBytes(httpRequest + headerLength, contentLength);
            }

			// Construct a request object and handle accordingly
            Request request(httpRequest, client);
            handleRequest(request);
			client.stop();
        } 
    }

    void on(const char * path, const char * method, RequestFunctionPointer callback) {
        RequestHandler handler;
        handler.path = path;
        handler.method = method;
        handler.callback = callback;
        handlers.append(handler);
    }

    void get(const char * path, RequestFunctionPointer callback) {
        on(path, "GET", callback);
    }

    void post(const char * path, RequestFunctionPointer callback) {
        on(path, "POST", callback);
    }

	//===========================================================
	// [+_+] Search for the coresponding request handler and call the callback function
	//===========================================================
    void handleRequest(Request & req) {
        Response res(req.client);
        for (int i = 0; i < handlers.size; i++) {
            if (strcmp(handlers[i].path, req.path) == 0 && strcmp(handlers[i].method, req.method) == 0) {
                handlers[i].callback(req, res);
                return;
            }
        }

        WiFiClient & client = req.client;
        client.println(F("HTTP/1.1 404 Not Found"));
        client.println(F("Connection: close"));
        client.println();
    }

    void printWiFiStatus() {
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

    void printMacAddress(byte mac[]) {
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
};