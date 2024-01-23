// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "client.h"
#include "server.h"



int main()
{
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	int choice;

	do{
		std::cout<<("would you like to access a Server or Client?\n");
		std::cout<<("1> Server\n");
		std::cout<<("2> Client\n");
		std::cin >> choice;
	} while (choice != 1 && choice != 2);

	//Server
	if (choice == 1)
	{
		std::cout << ("welcome server\n");

		server myServer;
		myServer.StartServer();

	}

	//Client
	if (choice == 2)
	{
		std::cout << ("welcome client!\n");

		client myClient;
		myClient.StartClient();
	}

	return WSACleanup();
}

