#include "handler.h"

int handler::tcp_recv_whole(SOCKET s, char* buf, int len)
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

int handler::tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
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

int handler::readMessage(char* buffer, int32_t _size)
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

int handler::sendMessage(char* data, int32_t length)
{
	if (length < 0 || length > 255) {
		return PARAMETER_ERROR;
	}

	uint8_t size;

	int result = tcp_send_whole(comSocket, (char*)&size, 1);
	if (result == SOCKET_ERROR) {
		std::cout << "SOCKET ERROR\n";
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