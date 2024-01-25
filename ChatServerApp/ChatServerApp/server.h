#pragma once
#include "platform.h"
#include "definitions.h"
#include "handler.h"
#include "client.h"

class server
{
	client myClient;

	SOCKET listenSocket;

	fd_set masterSet; // containing all the sockets that the server is monitoring
	fd_set readySet;  // containing the sockets that are ready for reading or writing

	uint16_t port;
	int capacity;

	std::string hostname;

	std::vector<SOCKET> connectedSockets; // to store client sockets

public:
	handler msgHandler;
	char commandChar;

	int StartServer();
	int init(uint16_t port);
	void ProcessCommand(char* command, SOCKET clientSocket);
	void stop();

};

