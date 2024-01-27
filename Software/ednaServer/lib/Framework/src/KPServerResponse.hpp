
#pragma once

#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <WiFi101.h>

#include <map>
#include <functional>

#include <KPFoundation.hpp>
#include <KPDataStoreInterface.hpp>

struct Response {
private:
	int status		   = 200;
	bool headerPending = true;
	std::map<const char *, const char *> headers{
		{"Content-Type", "text/html; charset=UTF-8"},
		{"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
		{"Access-Control-Allow-Origin", "*"},
		{"Connection", "Keep-Alive"},
		{"Content-Language", "en-us"}};

	std::function<void()> _notFound;

	void sendHeader() {
		client.printf("HTTP/1.1 %d %s\r\n", status, statusText(status));
		for (auto & p : headers) {
			client.printf("%s: %s\r\n", p.first, p.second);
		}

		client.print("\r\n");
		headerPending = false;
	}

	const char * statusText(int code) const {
		switch (code) {
		case 200:
			return "OK";
		default:
			return "Not Found";
		}
	}

public:
	WiFiClient & client;
	const size_t TCP_LIMIT;
	Response(WiFiClient & client, const size_t tcp_limit) : client(client), TCP_LIMIT(tcp_limit) {
		_notFound = [&]() {
			client.println("HTTP/1.1 404 Not Found");
			client.println("Connection: close");
			client.print("\r\n");
		};
	}

	void setHeader(const char * key, const char * value) {
		for (auto p : headers) {
			if (strcmpi(p.first, key) == 0) {
				headers[p.first] = value;  // update
				return;
			}
		}

		headers[key] = value;
	}

	template <typename T>
	size_t send(const T & data) {
		if (headerPending) {
			sendHeader();
		}

		return client.print(data);
	}

	template <size_t N>
	size_t send(const char (&data)[N]) {
		if (N > TCP_LIMIT) {
			PrintConfig::setPrintVerbose(Verbosity::info);
			println("Warning: data exeeds TCP limit. All or some of it may be lost.");
			println("Try to reduce the amount of data sent at once to be below ", TCP_LIMIT,
					" bytes");
			PrintConfig::setDefaultVerbose();
		}

		if (headerPending) {
			sendHeader();
		}

		return client.print(data);
	}

	size_t json(const JsonDocument & doc) {
		if (headerPending) {
			setHeader("Content-Type", "application/json");
			sendHeader();
		}

		WriteBufferingClient buffer(client, 64);
		return serializeJson(doc, buffer);
	}

	size_t sendFile(const char * filepath, KPDataStoreInterface & store) {
		if (headerPending) {
			setHeader("Transfer-Encoding", "chunked");
			sendHeader();
		}

		// Chunk encoding
		const size_t bufferSize = 1400;
		char buffer[bufferSize]{0};
		int charsRead = 0;
		size_t total  = 0;
		while ((charsRead = store.loadContentOfFile(filepath, buffer, bufferSize)) > 0) {
			client.printf("%X\r\n", charsRead);
			client.write(buffer, charsRead);
			client.flush();
			total += charsRead;
		}

		// Chunk terminator
		if (charsRead == 0) {
			client.print("0\r\n");
		}

		return total + 1;
	}

	void end() {
		if (headerPending) {
			sendHeader();
		}

		client.print("\r\n\r\n");
		client.stop();
	}

	void onNotFound(std::function<void()> callback) {
		_notFound = callback;
	}

	void notFound() {
		_notFound();
	}
};