#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock

#include <iostream>
#include <string>
#include "client.h"


int tcp_recv_whole(SOCKET s, char* buf, int len)
{
	int total = 0;
	do
	{
		int ret = recv(s, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}

int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}

void client::StartClient() {

	std::cout << "Client is running...\n";
	std::cout << "Enter the server address: \n";
	std::cin >> address;

	std::cout << "Enter the server port: \n";
	std::cin >> port;

	int initResult = init(port, const_cast<char*>(address.c_str()));
	if (initResult == SUCCESS) {
		std::cout << "Successfully connected to the server.\n";

		std::cin.ignore(); // Ignore the newline character left in the buffer
		std::cout << "Enter your message: ";
		std::string userMessage;
		std::getline(std::cin, userMessage);

		// Call the sendMessage function with the user's message
		int sendResult = sendMessage(const_cast<char*>(userMessage.c_str()), userMessage.length());

		// Check the result of sending the message
		if (sendResult == SUCCESS) {
			std::cout << "Message sent successfully.\n";
		}
		else {
			std::cout << "Error sendinsage. Code: " << sendResult << "\n";

		}
	}
	else {
		std::cout << "Error connecting to the server. Code: " << initResult << "\n";
	}
}

int client::init(uint16_t port, char* address)
{
	if (inet_addr(address) == INADDR_NONE) {
		return ADDRESS_ERROR;
	}

	comSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (comSocket == INVALID_SOCKET) {
		return SETUP_ERROR;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(address);
	serverAddr.sin_port = htons(port);

	int result = connect(comSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAECONNREFUSED) {
			return CONNECT_ERROR;
		}
		else if (WSAGetLastError() == WSAESHUTDOWN) {
			return SHUTDOWN;
		}
		else {
			return SETUP_ERROR;
		}
	}

	return SUCCESS;
}

int client::readMessage(char* buffer, int32_t _size)
{
	uint8_t size = 0;
	int result = tcp_recv_whole(comSocket, (char*)&size, 1);
	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	if (size > _size) {
		return PARAMETER_ERROR;
	}

	result = tcp_recv_whole(comSocket, (char*)buffer, size);
	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	return SUCCESS;
}

int client::sendMessage(char* data, int32_t length)
{
	if (length < 0 || length > 255) {
		return PARAMETER_ERROR;
	}

	uint8_t size;
	int result = tcp_send_whole(comSocket, (char*)&size, 1);

	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	result = tcp_send_whole(comSocket, data, size);

	if (result == SOCKET_ERROR) {
		return DISCONNECT;
	}
	else if (result == 0) {
		return SHUTDOWN;
	}

	return SUCCESS;
}

void client::stop()
{
	shutdown(comSocket, SD_BOTH);
	closesocket(comSocket);
}
