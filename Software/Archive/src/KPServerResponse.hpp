#pragma once
#include "KPFoundation.hpp"
#include "KPSDCard.hpp"
#include "WiFi101.h"

struct Response {
    int status = 200;
    String contentType = "text/html";
    bool isHeaderSent = false;

    Array<const char *> headers;

    WiFiClient & client;
    Response(WiFiClient & client) : client(client) {
		headers.append("Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS");
		headers.append("Access-Control-Allow-Origin: *");
	}

    void sendHeader() {
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

        client.print(F("Content-Type: "));
        client.println(contentType);
        client.println(F("Connection: keep-alive"));

        for (int i = 0; i < headers.size; i++) {
            client.println(headers[i]);
        }

        // End of header
        client.println();
    }

    void send(const char s[]) {
        sendHeader();
        const int bufferSize = 768;
        int amountLeft = strlen(s);
        for (int i = 0; amountLeft > 0; i++) {
            client.write(s + i * bufferSize, amountLeft > bufferSize ? bufferSize : amountLeft);
            amountLeft -= bufferSize;
        }
    }

    void send(Printable & p) {
        sendHeader();
        client.print(p);
    }

    void send(int i) {
        sendHeader();
        client.print(i);
    }

    void send(float f) {
        sendHeader();
        client.print(f);
    }

    void sendNewline() {
        sendHeader();
        client.println();
    }

    void end() {
        sendHeader();
        delay(5);
        client.stop();
    }

    void serveContentFromFile(const char * filepath, KPSDCard & card) {
        const int size = 4096;
        char buffer[size]{0};
        while (card.loadContentsOfFile(filepath, buffer, size)) {
            send(buffer);
        }
    }
};