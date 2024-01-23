#pragma once
#include "platform.h"
#include "definitions.h"
#include "handler.h"

class server
{
	handler msgHandler;

	SOCKET listenSocket;
	SOCKET comSocket;

	fd_set masterSet; // containing all the sockets that the server is monitoring
	fd_set readySet;  // containing the sockets that are ready for reading or writing

	uint16_t port;
	int capacity;
	char commandChar;

public:
	int StartServer();
	int init(uint16_t port);
	int readMessage(char* buffer, int32_t size);
	int sendMessage(char* data, int32_t length);
	void stop();

};

