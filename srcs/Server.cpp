#include "Server.hpp"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <stdexcept>
#include <cstring>
//#include <iostream>

Server::Server() : _listenFd(-1), _port(0), _root(""), _clients()  
{}

Server::Server(const Server& other) : _listenFd(other._listenFd), _port(other._port), _root(other._root), _clients(other._clients) {}

Server::~Server() {}

void	Server::createListeningSocket(void)
{
	this->_listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (this->_listenFd == -1)
		throw std::runtime_error("Server: socket() failed.");

	if (fcntl(this->_listenFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Server: fcntl() failed.");

	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) == -1)
	{
		close(_listenFd);
		throw std::runtime_error("Server: setsockopt() failed.");
	}

	return ;
	//setListeningSocketOptions(this->_listenFd);
}

void	Server::bindAndListen()
{
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t addrlen = sizeof(sockaddr_in);

	if (bind(this->_listenFd, (struct sockaddr*)&server_addr, addrlen) == -1)
	{
		close(this->_listenFd);
		throw std::runtime_error("Server: bind() failed.");
	}

	if (listen(this->_listenFd, BACKLOG) == -1) // 1156 listen system call marks as passive socket
	{
		close(this->_listenFd); //safe close
		throw std::runtime_error("Server: listen() failed.");
	}
	return ;
}

int	Server::getListenFd(void) const { return this->_listenFd; }
