#pragma once
#include <SPI.h>
#include <WiFi101.h>
#include "OPComponent.hpp"

class OPServer;

struct Request {
    char * method = nullptr;
    char * path = nullptr;
	char * header = nullptr;
    char * body = nullptr;
    WiFiClient & client;

    Request(char * httpRequest, int headerLength, WiFiClient & client) : client(client) {
        if (headerLength < 2) {
            return;
        }

        httpRequest[headerLength - 1] = 0; 	// Seperate body from header 
        httpRequest[headerLength - 2] = 0;	// Seperate body from header 
        method = strtok(httpRequest, " ");
        path = strtok(NULL, "\r\n");
        body = httpRequest + headerLength;
    }

	Request(char * httpRequest, WiFiClient & client) : client(client) {
		char * endOfHeader = strstr(httpRequest, "\r\n\r\n");
		endOfHeader[0] = 0;
		method = strtok(httpRequest, " ");
		path = strtok(NULL, " ");
		header = path + strlen(path) + 1;
		body = endOfHeader + 3;
	}
};

struct Response {
    int status = 200;
    String contentType = "text/html";
    bool isHeaderSent = false;

    WiFiClient & client;
    Response(WiFiClient & client) : client(client) {}

    void sendHeader();
    void send(const char s[]);
    void send(Printable & p);
    void send(int i);
    void send(float f);
	void sendNewline();
    void end();
};

using RequestFunctionPointer = void (*)(Request & req, Response & res);

struct RequestHandler {
    const char * path;
	const char * method;
    RequestFunctionPointer callback;
};

class OPServer : public OPComponent {
   public:
    char * homepage;
	
   public:
    const char * ssid;
    const char * pass;

    uint8_t status = WL_IDLE_STATUS;
    bool isRunning = false;

    int size = 0;
    RequestHandler handlers[10];

    WiFiServer server;
    OPServer(const char * name, const char ssid[], const char pass[]) 
		: OPComponent(name), ssid(ssid), pass(pass), server(80) {}

    void setup() override;
    void update() override;

    void begin();

	void sendHomepage(Request & req, Response & res) {
		res.send(homepage ? homepage : "");
		res.end();
	}

    void on(const char * path, const char * method, RequestFunctionPointer callback);
	void get(const char * path, RequestFunctionPointer callback);
	void post(const char * path, RequestFunctionPointer callback);
    void handleRequest(Request & req);

    void printWiFiStatus();
    void printMacAddress(byte mac[]);
};