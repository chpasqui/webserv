#include "../../include/Server.hpp"
#include "../../include/Session.hpp"
#include "../../include/Cgi.hpp"

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

// CONSTRUCTOR AND DESTRUCTOR

int serverSignal = 0;

Server::Server() : epollFd(-1),
				   _listenFds(),
				   _clients()
{
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];
	int nFd;

	while (!serverSignal)
	{
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, -1);
		for (int i = 0; i < nFd; i++)
		{
			int fd = events[i].data.fd;
			if (_listenFds.count(fd))
			{
				if (!acceptNewConnection(fd))
					continue;
			}
			else if (events[i].events & (EPOLLERR | EPOLLHUP))
				closeConnection(fd);
			else if (events[i].events & EPOLLIN)
				readFromClient(fd); // if pipecgi.count() --> read from pipe
			else if (events[i].events & EPOLLOUT)
				writeToClient(fd);
		}
	}
	std::cout << "signal to stop received" << std::endl;
}

bool Server::acceptNewConnection(int listeningFd)
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
		addToEpoll(this->epollFd, clientFd, EPOLLIN);
		// add the multiple serverconfigs in one fd and map with the host name (multiple config for one port)
		const ServerConfig *config = _listenFds[listeningFd];
		_clients.insert(std::make_pair(clientFd, new Client(clientFd, config)));
		std::cout << "new connection with " << clientFd << "." << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		closeConnection(clientFd);
		return false;
	}
	return (true);
}

// potentially rename to treatClientRequest
void Server::readFromClient(int clientFd)
{
	// check that there is a client
	Client &client = *(_clients[clientFd]); // _clients[6] will give us the Client class at clientFd = 6

	client.parseRequest();

	if (client.getParsingStatus() == CLOSEFD)
	{
		closeConnection(clientFd);
		return;
	}
	if (client.getParsingStatus() == COMPLETE)
	{
		bool isNew = false;
		Session &session = resolveSession(client.getRequest()->getHeader("cookie"), isNew);
		client.setResponse(session, isNew);
		client.getResponse()->build(*client.getRequest(), client.getServerConfig());
		// client.buildResponse(session, isNew);
		if (client.getResponse()->getCgiStatus() == true)
		{
			std::cout << "entered the case where it is cgi" << std::endl;
			// create pipes
			// addtoEpoll
			launchCgi(client);
		}
		else
		{
			if (!modifyEpoll(this->epollFd, clientFd, EPOLLOUT))
			{
				closeConnection(clientFd);
				return;
			}
		}
	}
}

//

// void	Server::readFromPipe(int pipeFd)
// {
// char buffer[1024];
// 	int contentLength = 0; // no content length --> because not obly the body
// 	do {
// 		contentLength = read(stdoutPipe[READ_END], buffer, 1024);
// 		if (contentLength > 0)
// 			responseBody.append(buffer, contentLength);
// 	} while(contentLength > 0);
// 	std::string contentLengthStr = "content-length=" + std::to_string(responseBody.size()) + '\n';
// 	responseBody.insert(0, contentLengthStr);
// 	close(stdoutPipe[READ_END]);
// }

void Server::launchCgi(const Client &client) const
{
	Cgi cgi;
	std::cout << "about to launch!" << std::endl;
	int pipeFd = cgi.launch(client.getResponse()->getInternalPath(), *client.getRequest(), *client.getServerConfig());
	addToEpoll(this->epollFd, pipeFd, EPOLLIN);
}

void Server::writeToClient(int fd)
{
	// the response will be called here
	// read the pipe end
	Client &client = *(_clients[fd]);
	std::string response = client.getResponseStr();
	send(fd, response.c_str(), response.size(), 0);
	std::cout << GREEN << "response sent to client succesfully!" << RESET << std::endl;
	closeConnection(fd);
}

void Server::closeConnection(int fd)
{
	delFromEpoll(this->epollFd, fd);
	if (close(fd) == -1)
		std::cerr << "Close failed" << std::endl; // check if we mannage this differently or not
	if (_clients.count(fd))
	{
		delete _clients[fd];
		_clients.erase(fd);
	}
}

// EPOLL HELPER FUNCTIONS

int Server::getEpollFd(void) const
{
	return (this->epollFd);
}

/**
 * @brief Destroy the Server:: Server object
 *
 */
Server::~Server()
{
	std::map<int, Client *>::iterator it = this->_clients.begin();
	for (; it != this->_clients.end(); ++it)
	{
		close(it->first);
	}
	_clients.clear();

	std::map<int, const ServerConfig *>::iterator secondIt = this->_listenFds.begin();
	for (; secondIt != this->_listenFds.end(); ++secondIt)
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
