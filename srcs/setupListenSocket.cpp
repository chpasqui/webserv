#include "../include/server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>

static int	setListeningSocketOptions(int *listenFd);

int	createListeningSocket(void)
{
	int listenFd; //TODO: close socketFd 

	listenFd = socket(PF_INET, SOCK_STREAM, 0); // page 1156 linux book
	if (listenFd == -1)
	{
		perror("Server: socket");
		return (-1);
	}
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("Server: Fcntl");
		return (-1);
	}
	if (setListeningSocketOptions(&listenFd) == -1)
	{
		close(listenFd);
		return (1);
	}

	return listenFd;
}

static int	setListeningSocketOptions(int *listenFd)
{
	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(*listenFd, SOL_SOCKET,
		SO_REUSEADDR, &optval, optlen) == -1) {
		perror("setsockopt");
		return (-1);
	}												// optlen = information for the setsockopt as optval is linked to SO_REUSEADDR
	return (0);
}

	int	bindListeningSocket(int *listenFd)
{
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(sockaddr_in));
	// Forcefully attaching the socket to the port 8080 (PORT)
	server_addr.sin_family = AF_INET; 					// Address family, AF_INET
	server_addr.sin_port = htons(PORT);					// htons = host to network short, used to set it to Network Byte Order (Big Endian)
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 	// host to network long
	socklen_t addrlen = sizeof(sockaddr_in);

	if (bind(*listenFd, (struct sockaddr*)&server_addr, addrlen) == -1)
	{
		close(*listenFd);
		perror("bind error");
		return (-1);
	}

	return (0);
}