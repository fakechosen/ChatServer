#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock

#include <iostream>
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
	// Implement client functionality here
	std::cout << "Client is running...\n";
	// Add code to communicate with the server
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
