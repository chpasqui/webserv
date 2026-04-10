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

void	Server::timeoutCheck(void) {
	time_t	currentTime;

	time(&currentTime);
	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (std::difftime(currentTime, it->second->getTimestamp()) == TIMEOUT)
		{
			std::cout << "Timeout on socket " << it->first << std::endl;
			//buildReponse();
			//this->modifyFdFlag(it->first, EPOLLOUT);
			//sendResponse();
			this->_clients.erase(it->first);
		}
	}
}

// SIGNAL HANDLING UTILS

void	Server::signalHandler(int signal) {
	if (signal == SIGINT)
		std::cout << "\nSIGINT signal detected, stopping server\n";
	else if (signal == SIGQUIT)
		std::cout << "\nSIGQUIT signal detected, stopping server\n";
	serverSignal = signal;
}
