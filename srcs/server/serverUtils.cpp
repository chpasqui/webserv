#include "../../include/Server.hpp"
#include "../../include/color.hpp"
#include "../../include/Cgi.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"

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


void Server:: updateTimeStamp(Client *client){
	time_t currentTime;
	time(&currentTime);

	client->setTimestamp(currentTime);
}

void	Server::timeoutCheck(void) {
	time_t	currentTime;
	time(&currentTime);

	std::map<int, Client*>::iterator it = _clients.begin();

	while (it != _clients.end()) {
		Client* client = it->second;
		LOG_DEBUG(C_CSTR << "client fd=" << it->first << " config=" << client->getServerConfig());
		if (std::difftime(currentTime, client->getTimestamp()) >= client->getServerConfig()->getClientTimeout()){
			if (client->getResponse() == NULL)
				client->setResponse();
			client->getResponse()->setError(GATEWAY_TIMEOUT, "timeoutCheck: response is not NULL");
			if (client->getCgi()){
				if (!addToEpoll(_epollFd, it->first, EPOLLOUT)){
					++it;
					closeConnection(client);
					continue;
				}
			}
			else {
				if(!modifyEpoll(_epollFd, it->first, EPOLLOUT)){
					++it;
					closeConnection(client);
					continue ;
				}
				std::cout << BLUE << "Connection closed" << RESET << std::endl;
			}
			updateTimeStamp(client);
			++it;
		}
		else
			++it;
	}
}

// SIGNAL HANDLING UTILS

void	Server::signalHandler(int signal) {
	if (signal == SIGINT)
		std::cout << "\nSIGINT signal detected, stopping server\n";
	// else if (signal == SIGQUIT)
	// 	std::cout << "\nSIGQUIT signal detected, stopping server\n"; 
	serverSignal = signal;
}