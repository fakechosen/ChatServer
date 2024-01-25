#include "server.h"
#include <WS2tcpip.h>
#define INET6_ADDRSTRLEN 46

int server::StartServer() {

	//local host name
	gethostname(const_cast<char*>(hostname.c_str()), 128);
	std::cout << "Host Name: " << hostname.c_str() << "\n";

	struct addrinfo hints, * res, * p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // Use AF_INET for IPv4, AF_INET6 for IPv6
	hints.ai_socktype = SOCK_STREAM;

	//get address
	status = getaddrinfo(const_cast<char*>(hostname.c_str()), NULL, &hints, &res);

	std::cout << "IP addresses for " << const_cast<char*>(hostname.c_str()) << ":\n";
	for (p = res; p != NULL; p = p->ai_next) {
		void* addr;
		const char* ipver;
		// get the pointer to the address itself,
		  // different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else { // IPv6
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		std::cout << " " << ipver << ": " << ipstr << "\n";
	}
	freeaddrinfo(res); // free the linked list


	std::cout << ("------------------------------------\n");

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
	
	std::cout << ("------------------------------------\n");
	std::cout << "{ server initialized }\n";


	FD_ZERO(&masterSet); //initializing the socket
	FD_SET(listenSocket, &masterSet); //add the listening socket to masterSet

	while (true) {

		readySet = masterSet; // copy fd_set masterSet to fd_readySet

		timeval timeout{};
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int readyfd = select(NULL, &readySet, NULL, NULL, &timeout);

		if (readyfd == SOCKET_ERROR) {
			std::cerr << "Error in select()\n";
			break;
		}

		for (int i = 0; i < readyfd; i++)
		{
			if (readySet.fd_array[i] == listenSocket) { //checking if there is a listensocket in readySet
				if (FD_ISSET(listenSocket, &readySet)) {
					SOCKET clientSocket = accept(listenSocket, NULL, NULL);
					if (clientSocket != INVALID_SOCKET) {
						FD_SET(clientSocket, &masterSet);
						connectedSockets.push_back(clientSocket); // Add new client socket to the vector
						std::cout << "\n{ connection to new client established }\n";

						std::string responseMessage = "Welcome to the Server!\nTo issue commands use ";
						responseMessage += commandChar;
						msgHandler.sendMessage(clientSocket, const_cast<char*>(responseMessage.c_str()), strlen(const_cast<char*>(responseMessage.c_str())));
					}
					else {
						std::cerr << "Error in accept()\n";
						break;
					}
				}
			}
			else {
				for (SOCKET clientSocket : connectedSockets) {
					if (FD_ISSET(clientSocket, &readySet)) {
						char buffer[256]; //max size of the message
						int readNum = msgHandler.readMessage(clientSocket, buffer, sizeof(buffer));

						if (readNum == DISCONNECT || readNum == SHUTDOWN) {
							FD_CLR(clientSocket, &masterSet);
							closesocket(clientSocket);
							std::cout << "\n{ client disconnected }\n";
							// Remove the disconnected client socket from the vector
							auto it = std::find(connectedSockets.begin(), connectedSockets.end(), clientSocket);
							if (it != connectedSockets.end()) {
								connectedSockets.erase(it);
							}
						}
						else if (readNum == PARAMETER_ERROR) {
							std::cerr << "Error in reading message.\n";
						}
						else {
							std::cout << "[Received message]: " << buffer << "\n";
							if (buffer[0] == commandChar) {
								ProcessCommand(buffer, clientSocket);
							}
						}
					}
				}
			}
		}
	}

	return SUCCESS;
}

void server::ProcessCommand(char* command, SOCKET clientSocket) {
	std::cout << "\n-----command request initiated by the client-----\n";

	std::string cmdStr(command);

	if (cmdStr.size() > 1 && cmdStr[0] == commandChar) {

		std::string actualCommand = cmdStr.substr(1); // extracting the command removing the '/'

		if (actualCommand == "help") {
			std::string cmdmsg = "Available commands:\n/help - Shows list of the commands available\n/register {username} {password} - lets user(s) register themselves to the server\n";
			std::cout << cmdmsg;
			msgHandler.sendMessage(clientSocket,const_cast<char*>(cmdmsg.c_str()), strlen(const_cast<char*>(cmdmsg.c_str())));

		}
		else {
			std::string cmdmsg = " ! UNKNOWN COMMAND ! \n";
			cmdmsg += "( /" + actualCommand + " ) does not exist, Try Again!";
			std::cout << cmdmsg;
			msgHandler.sendMessage(clientSocket, const_cast<char*>(cmdmsg.c_str()), strlen(const_cast<char*>(cmdmsg.c_str())));
			std::cout << "Unknown command: " << actualCommand << "\n";
		}
	}
	else {
		//normal msg
	}


}

void server::stop()
{
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	for (SOCKET clientSocket : connectedSockets) {
		shutdown(clientSocket, SD_BOTH);
		closesocket(clientSocket);
	}
	connectedSockets.clear(); // Clear the vector after closing all sockets
}
