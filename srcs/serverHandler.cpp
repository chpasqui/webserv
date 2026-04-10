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

bool	Server::addNewConnection(int listeningFd)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientFd = accept(listeningFd, (struct sockaddr *)&client_addr, &client_len);
	if (clientFd == -1) // here we did a perror and not a try catch?
	{
		perror("accept error");
		return false;
	}
	ServerConfig *config = _listenFds[listeningFd];
	_clients.insert(std::make_pair(clientFd, new Client(clientFd, config, new Request())));
	try
	{
		addToEpoll(this->epollFd, clientFd, EPOLLIN); // addtoEPoll and all the epoll functions send back an exception which doesnt work i need to fix that
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		removeConnection(clientFd);
		return false;
	}
	return (true);
}
