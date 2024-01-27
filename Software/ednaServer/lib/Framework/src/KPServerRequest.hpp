#pragma once
#include "KPFoundation.hpp"
#include "WiFi101.h"

struct Request {
	char * method = nullptr;
	char * path	  = nullptr;
	char * header = nullptr;
	char * body	  = nullptr;
	WiFiClient & client;

	Request(char * httpRequest, WiFiClient & client) : client(client) {
		char * endOfHeader = strstr(httpRequest, "\r\n\r\n");
		endOfHeader[0]	   = 0;
		method			   = strtok(httpRequest, " ");
		path			   = strtok(NULL, " ");
		header			   = path + strlen(path) + 1;
		body			   = endOfHeader + 3;
	}

	void json() {}
};