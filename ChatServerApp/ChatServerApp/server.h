#pragma once
#include "platform.h"
#include "definitions.h"
#include "handler.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <filesystem>
#include <thread>

class server
{


	fd_set masterSet; // containing all the sockets that the server is monitoring
	fd_set readySet;  // containing the sockets that are ready for reading or writing

	uint16_t port;
	int capacity;

	std::string hostname;

	SOCKET listenSocket;
	std::vector<SOCKET> connectedSockets; // to store client sockets

	std::unordered_map<std::string, std::string> userCredentials; // Map to store usernames and passwords
	std::unordered_map<SOCKET, std::string> loggedInUsers; // Map to store logged-in users

	int registeredUsersCount;

	std::ofstream userCommandsLogFile;
	std::ofstream publicMessagesLogFile;



public:
	handler msgHandler;
	char commandChar;

	int StartServer();
	int init(uint16_t port);

	bool IsRegistrationLimitReached();

	void ProcessCommand(char* command, SOCKET clientSocket);
	void LoginCommand(std::string& username, std::string& password, SOCKET clientSocket);
	void LogoutCommand(SOCKET clientSocket);
	void SendCommand(const std::string& command, SOCKET senderSocket);
	void RelayMessage(const std::string& message, SOCKET senderSocket);
	void GetListCommand(SOCKET senderSocket);
	void StartUDPBroadcast(int port, const std::string& ipAddress);
	void stop();

	void LogUserCommand(const std::string& command, const std::string& username); //func to log commands
	void LogPublicMessage(const std::string& message, const std::string& sender); //func to log msgs
	void ClearLogs();
	std::string getTime();

};

