#include "server.h"

int server::StartServer() {
	std::cout << "Enter TCP Port number: ";
	std::cin >> port;

	std::cout << "Enter chat capacity: ";
	std::cin >> capacity;

	std::cout << "Enter the command character (default is / ): ";
	std::cin >> commandChar;

	int initResult = init(port);

	// error checking
	if (initResult == SUCCESS) {
		std::cout << "Server is set up successfully.\n";
		return SUCCESS;
	}
	else {
		std::cerr << "Error setting up the server. Code: " << initResult << "\n";
		return initResult;
	}
}

int server::init(uint16_t port)
{
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	int result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == BIND_ERROR)
	{
		return  BIND_ERROR;
	}

	result = listen(listenSocket, 1);
	if (result == SETUP_ERROR)
	{
		return SETUP_ERROR;
	}

	//comSocket = accept(listenSocket, NULL, NULL);
	//if (comSocket == INVALID_SOCKET)
	//{
	//	if (WSAGetLastError() == WSAESHUTDOWN)
	//	{
	//		return SHUTDOWN;
	//	}
	//	else
	//	{
	//		return CONNECT_ERROR;
	//	}
	//}

	std::cout << "{ server initialized }\n";

	FD_ZERO(&masterSet); //initializing the socket
	FD_SET(listenSocket, &masterSet); //add the listening socket to masterSet

	while (true) {
		readySet = masterSet; // copy fd_set masterSet to fd_readySet

		timeval timeout{};
		timeout.tv_sec = 5;
		timeout.tv_sec = 0;

		int readyfd = select(NULL, &readySet, NULL, NULL, &timeout);

		if (readyfd == SOCKET_ERROR) {
			std::cerr << "Error in select()\n";
			break;
		}

		for (int i = 0; i < readyfd; i++)
		{
			if (readySet.fd_array[i] == listenSocket) { //checking if there is a listensocket in readySet
				if (FD_ISSET(listenSocket, &readySet)) {
					comSocket = accept(listenSocket, NULL, NULL);
					if (comSocket != INVALID_SOCKET) {
						FD_SET(comSocket, &masterSet);
						std::cout << "{ connection to new client established }\n";
					}
					else {
						std::cerr << "Error in accept()\n";
						break;
					}
				}
			}
			else if (FD_ISSET(comSocket, &readySet)) {
				char buffer[256]; //max size of the message
				int readNum = readMessage(buffer, sizeof(buffer));

				if (readNum == DISCONNECT || readNum == SHUTDOWN) {
					FD_CLR(comSocket, &masterSet);
					closesocket(comSocket);
					std::cout << "{ client disconnected }\n";
				}
				else if (readNum == PARAMETER_ERROR) {
					std::cerr << "Error in reading message.\n";
				}
				else {
					std::cout << "[Received message]: " << buffer << "\n";
				}
			}
		}

	}

	
	return SUCCESS;
}

int server::readMessage(char* buffer, int32_t _size)
{
	uint8_t size = 0;
	int result = msgHandler.tcp_recv_whole(comSocket, (char*)&size, 1);

	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	if (size > _size) {
		return PARAMETER_ERROR;
	}

	result = msgHandler.tcp_recv_whole(comSocket, (char*)buffer, size);
	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	return SUCCESS;
}

int server::sendMessage(char* data, int32_t length)
{
	if (length < 0 || length > 255) {
		return PARAMETER_ERROR;
	}

	uint8_t size;

	int result = msgHandler.tcp_send_whole(comSocket, (char*)&size, 1);
	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	result = msgHandler.tcp_send_whole(comSocket, data, size);
	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	return SUCCESS;
}

void server::stop()
{
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	shutdown(comSocket, SD_BOTH);
	closesocket(comSocket);
}
