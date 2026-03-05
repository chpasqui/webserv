#include "../include/server.hpp"
#include "../include/main.hpp"
#include <string>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <netinet/in.h>

#define BACKLOG 5

int createSocket(void)
{
	int socketFd;
	
	// Creating socket file descriptor
	socketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (socketFd == -1)
		perror("server: socket");
	
	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(socketFd, SOL_SOCKET,
		SO_REUSEADDR, &optval, optlen) == -1) {
		perror("setsockopt");
	}												// optlen = information for the setsockopt as optval is linked to SO_REUSEADDR
			
	// Forcefully attaching the socket to the port 8080 (PORT)
	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(sockaddr_in));
	my_addr.sin_family = AF_INET; 					// Address family, AF_INET
	my_addr.sin_port = htons(PORT);					// htons = host to network short, used to set it to Network Byte Order (Big Endian)
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 	// host to network long

	socklen_t addrlen = sizeof(sockaddr_in);
	if (bind(socketFd, (struct sockaddr*)&my_addr, addrlen) == -1)
		perror("bind error");
	
	int ok  = 0;
	while (!ok)
	{
		ok = listen(socketFd, BACKLOG);
		if (ok == -1)
			perror("listen error");
	}
	return (0);
}
