#include "../include/Server.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

/**
 * @brief Creates a TCP listening socket, sets it to non blocking mode via fcntl()
 * and enables the SO_REUSEADDR to alllow the rebinding to another port in the 
 * TIME_WAIT state
 * 
 * @return the listenfd socket fd, ready to be bound to a port
 */
int Server::openListeningSocket() {
	int listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
	{
		throw std::runtime_error("Server: socket() failed.");
	}
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: fcntl() failed.");
	}
	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: setsockopt() failed.");
	}
	std::cout << "Server::createlisteningSocket listenFd " << listenFd << " is created" <<  std::endl;
	return listenFd;
}

/**
 * @brief Binds all the listening socket to the given port on all available 
 * network interfaces INADDR_ANY and marks them as passive via
 * the listen() syscall, allowing it to queue up to BACKLOG incoming connections 
 * 
 * @param listenFd listening socket to be bound, closed upon failure
 * @param port which is the serverConfigVec[i].getport() so that we bind the
 * listen fd to that specific port
 */
void	Server::bindAndListen(int &listenFd, int port) const {
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t addrlen = sizeof(sockaddr_in);

	if (bind(listenFd, (struct sockaddr*)&server_addr, addrlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: bind() failed.");
	}
	if (listen(listenFd, BACKLOG) == -1) // 1156 listen system call marks as passive socket
	{
		close(listenFd);
		throw std::runtime_error("Server: listen() failed.");
	}
	return ;
}
