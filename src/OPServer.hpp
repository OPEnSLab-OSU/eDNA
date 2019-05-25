#pragma once
#include <SPI.h>
#include <WiFi101.h>
#include "OPComponent.hpp"

struct Request {
    char * method = nullptr;
    char * url = nullptr;
    char * body = nullptr;
    WiFiClient & client;

    Request(char * httpRequest, WiFiClient & client) : client(client) {
        method = strtok(httpRequest, " ");
        url = strtok(NULL, " ");
    }
};

struct Response {
    int status = 200;

    String contentType = "text/html";
    String redirectURL;

    bool isHeaderSent = false;
    bool willRedirect = false;

    WiFiClient & client;
    Response(WiFiClient & client) : client(client) {}

    void sendHeader() {
        if (isHeaderSent) return;
        isHeaderSent = true;

        // HTTP Status
        switch (status) {
        case 200:
            client.println(F("HTTP/1.1 200 OK"));
            break;
        case 302:
            client.println(F("HTTP/1.1 302 Redirect"));
            break;
        default:
            client.println(F("HTTP/1.1 404 Not Found"));
            break;
        }

        // Content-Type
        if (contentType == "text/plain") {
            client.println(F("Content-Type: text/plain"));
        } else {
            client.println(F("Content-Type: text/html"));
        }

        // Connection
        client.println(F("Connection: close"));
        client.println();
    }

    void send(const char s[]) {
        sendHeader();
        const int size = 768;
        for (unsigned int i = 0; i < strlen(s); i += size) {
            char part[size + 1] = {0};
            strncpy(part, s + i, size);
            client.print(part);
        }
		client.println();
    }

    void send(Printable & p) {
        sendHeader();
        client.println(p);
    }

    void send(int i) {
        sendHeader();
        client.println(i);
    }

    void send(float f) {
        sendHeader();
        client.println(f);
    }

    void end() {
        sendHeader();
        delay(1);
        client.stop();
    }
};

using RequestFunctionPointer = void (*)(Request & req, Response & res);

struct RequestHandler {
    const char * url;
    RequestFunctionPointer callback;
};

class OPServer : public OPComponent {
   public:
    const char * ssid;
    const char * pass;

    uint8_t status = WL_IDLE_STATUS;
    bool isRunning = false;

    int size = 0;
    RequestHandler handlers[10];

    WiFiServer server;
    OPServer(String name, const char ssid[], const char pass[]) : OPComponent(name), ssid(ssid), pass(pass), server(80) {}

    void setup() override {
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
    }

    void begin() {
        isRunning = true;
    }

    void update() override {
        if (isRunning == false) {
            return;
        }

        // WiFi status has changed
        if (status != WiFi.status()) {
            if ((status = WiFi.status()) == WL_AP_CONNECTED) {
                byte remoteMac[6];
                WiFi.APClientMacAddress(remoteMac);
                Serial.print(F("Device connected to AP, MAC address: "));
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
        if (!client || !client.connected() || client.available() == -1) {
            client.stop();
            return;
        }

        const int size = 512;
        char httpRequest[size + 1] = {0};
        client.read((byte *) httpRequest, size);

        // Serial.println(httpRequest);
        // for (int i = 0; i < size; i++) {
        //     Serial.print((int) httpRequest[i]);
        // }
		// Serial.println();

        Request request(httpRequest, client);
        handleRequest(request);
    }

    void on(const char url[], RequestFunctionPointer callback) {
        handlers[size].url = url;
        handlers[size].callback = callback;
        size++;
    }

    void handleRequest(Request & req) {
        Response res(req.client);
        for (int i = 0; i < size; i++) {
            if (strcmp(handlers[i].url, req.url) == 0) {
                handlers[i].callback(req, res);
                break;
            }
        }

        // if (res.willRedirect) {
        // 	req.url = res.redirectURL;
        // 	res.willRedirect = false;
        // 	handleRequest(req);
        // }

        WiFiClient & client = req.client;
        client.println(F("HTTP/1.1 404 Not Found"));
        client.println(F("Connection: close"));
        client.println();
        client.stop();
    }

    static void printWiFiStatus() {
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

        // Web browser:
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