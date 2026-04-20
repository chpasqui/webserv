#include "../include/Server.hpp"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <string>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <csignal>
#include <signal.h>
#include "../include/color.hpp"


#define MAX_EVENTS 1024
#define TIMEOUT 10

int serverSignal = 0;

Server::Server(char **env) : 
		epollFd(-1),
		env(env),
		_listenFds(),
		_clients()
{
}

/**
 * @brief Destroy the Server:: Server object
 * 
 */
Server::~Server() {
	std::map<int, Client*>::iterator it = this->_clients.begin();
	for ( ; it != this->_clients.end(); ++it)
	{
		close(it->first);
	}
	_clients.clear();

	std::map<int, const ServerConfig*>::iterator secondIt = this->_listenFds.begin();
	for ( ; secondIt != this->_listenFds.end(); ++secondIt)
	{
		close(secondIt->first);
	}
	_listenFds.clear();
	if (this->epollFd != -1)
	{
		close(this->epollFd);
		this->epollFd = -1;
	}
}

// EPOLL HELPER FUNCTIONS

int		Server::getEpollFd(void) const
{
	return (this->epollFd);
}
