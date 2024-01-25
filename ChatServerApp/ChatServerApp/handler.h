#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock

#include <winsock2.h>
#include <iostream>
#include "platform.h"
#include "definitions.h"
#include <vector>

#pragma comment(lib,"Ws2_32.lib")

class handler
{


public:


	int tcp_recv_whole(SOCKET s, char* buf, int len);
	int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length);
	int readMessage(SOCKET clientSocket, char* buffer, int32_t size);
	int sendMessage(SOCKET clientSocket, char* data, int32_t length);
};

