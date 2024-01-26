#pragma once
#include "platform.h"
#include "definitions.h"
#include "handler.h"
#include "client.h"
#include <unordered_map>

class server
{
	client myClient;

	fd_set masterSet; // containing all the sockets that the server is monitoring
	fd_set readySet;  // containing the sockets that are ready for reading or writing

	uint16_t port;
	int capacity;

	std::string hostname;

	SOCKET listenSocket;
	std::vector<SOCKET> connectedSockets; // to store client sockets

	std::unordered_map<std::string, std::string> userCredentials; // Map to store usernames and passwords

public:
	handler msgHandler;
	char commandChar;

	int StartServer();
	int init(uint16_t port);
	void ProcessCommand(char* command, SOCKET clientSocket);

	void stop();

};

