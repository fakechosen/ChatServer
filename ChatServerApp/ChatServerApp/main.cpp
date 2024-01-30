// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "server.h"

server myServer;

int main()
{
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	//Server
	std::cout << ("------------------------------------\n");
	std::cout << ("Welcome to the Server\n");
	std::cout << ("------------------------------------\n");

	myServer.StartServer();

	return WSACleanup();
}

