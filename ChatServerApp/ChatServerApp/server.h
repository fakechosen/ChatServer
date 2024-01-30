#pragma once
#include "platform.h"
#include "definitions.h"
#include "handler.h"
#include "client.h"
#include <unordered_map>
#include <unordered_set>

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
	std::unordered_map<SOCKET, std::string> loggedInUsers; // Map to store logged-in users
	

public:
	handler msgHandler;
	char commandChar;

	int StartServer();
	int init(uint16_t port);
	void ProcessCommand(char* command, SOCKET clientSocket);
	void LoginCommand(std::string& username, std::string& password, SOCKET clientSocket);
	void LogoutCommand(SOCKET clientSocket);
	void SendCommand(const std::string& command, SOCKET senderSocket);
	void stop();

};

