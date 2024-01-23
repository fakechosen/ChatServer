#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock

#include <winsock2.h>
#include <iostream>

#pragma comment(lib,"Ws2_32.lib")

class handler
{
public:
	int tcp_recv_whole(SOCKET s, char* buf, int len);
	int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length);
};

