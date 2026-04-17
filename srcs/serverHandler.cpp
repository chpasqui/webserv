#include "../include/Server.hpp"
#include "../include/Cgi.hpp"

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

	try
	{
		addToInterestList(this->epollFd, clientFd, EPOLLIN);
		const ServerConfig *config = _listenFds[listeningFd];
		_clients.insert(std::make_pair(clientFd, new Client(clientFd, config, new Request())));
		std::cout <<"new connection with " << clientFd << "." << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		removeConnection(clientFd);
		return false;
	}
	return (true);
}


void	Server::readFromSocket(int fd)
{
	Client& client = *(_clients[fd]); // _clients[6] will give us the Client class at fd = 6

	client.parseRequest();
	if (client.getClientStatus() == CLOSEFD)
	{
		removeConnection(fd);
	}
	if (client.getClientStatus() == COMPLETE)
	{
		Cgi cgi(this->originalEnv);
		cgi.handleCgi(client, "/var/www/cgi-bin/foo.py/user/42");
		// build response --> in here Cgi 
		if (!modifyEpoll(this->epollFd, fd, EPOLLOUT))
		{
			removeConnection(fd);
			// not sure about how to react to this ? 
		}
	}
}

void	Server::writeToSocket(int fd)
{

	removeConnection(fd);
}

void	Server::removeConnection(int fd)
{
	if (close(fd) == -1)
		std::cerr << "Close failed" << std::endl; // check if we mannage this differently or not
	if (_clients.count(fd))
	{
		delete _clients[fd];
		_clients.erase(fd);
	}
}
