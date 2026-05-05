#include "../../include/Server.hpp"
#include "../../include/Session.hpp"
#include "../../include/Cgi.hpp"

#include <algorithm>
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
#define BUFFER_SIZE 1024

// CONSTRUCTOR AND DESTRUCTOR

int serverSignal = 0;

Server::Server() : _epollFd(-1),
					_bodyOffset(0),
				   _listenFds(),
				   _clients(),
				   _pipeFd()
{
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];
	int nFd;

	while (!serverSignal)
	{
		nFd = epoll_wait(this->_epollFd, events, MAX_EVENTS, -1);
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
			{
				if (this->_pipeFd.count(fd))
				{
					std::cout << "entered the case of EPOLLIN and pipe" << std::endl;
					Client *client = this->_pipeFd[fd];
					
					readFromPipe(fd);
					std::cout << "after read" << std::endl;
					//std::cout << "tempresponse is " << _tempResponse << std::endl;
					client->getResponse()->buildCgiResponse(this->_tempResponse);
					std::cout << "after" << std::endl;
					std::cout << "fd is " << fd << std::endl;
					if (!modifyEpoll(this->_epollFd, this->_pipeFd[fd]->getClientFd(), EPOLLOUT))
					{
						closeConnection(this->_pipeFd[fd]->getClientFd());
						return;
					}
					this->_pipeFd.erase(fd);
					std::cout << "after EPOLLOUT mdif" << std::endl;
				}
				else
					readFromClient(fd);
			}
			else if (events[i].events & EPOLLOUT)
			{
				if (this->_pipeFd.count(fd))
				{
					std::cout << "in the write to pipe function" << std::endl;
					Client *client = this->_pipeFd[fd];
					writeToPipe(*client, fd);
				}
				else
				{
					std::cout << "write to client entered" << std::endl;
					writeToClient(fd);
				}
			}
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
		addToEpoll(this->_epollFd, clientFd, EPOLLIN);
		// add the multiple serverconfigs in one fd and map with the host name (multiple config for one port)
		const ServerConfig *config = _listenFds[listeningFd];
		_clients.insert(std::make_pair(clientFd, new Client(clientFd, config)));
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
			launchCgi(client);
		}
		else
		{
			if (!modifyEpoll(this->_epollFd, clientFd, EPOLLOUT))
			{
				closeConnection(clientFd);
				return;
			}
		}
	}
}

void Server::readFromPipe(int stdoutPipeReadFd)
{
	char buffer[BUFFER_SIZE];
	ssize_t nbytes = 0;
	nbytes = read(stdoutPipeReadFd, buffer, BUFFER_SIZE);
	if (nbytes < 0)
	{
		perror("Error: ");
		return;
	}
	if (nbytes > 0)
	{
		this->_tempResponse.append(buffer);
		return;
	}
	if (nbytes == 0)
	{
		std::cout << "entered the case of EOF in read" << std::endl;
		delFromEpoll(_epollFd, stdoutPipeReadFd);
		return; // maybe put in server so that we send immediately to response 
	}
}

void Server::launchCgi(Client &client)
{
	client.setCgi();
	Cgi *cgi = client.getCgi();
	cgi->launch(client.getResponse()->getInternalPath(), *client.getRequest(), *client.getServerConfig());
	if (cgi->getStdinWriteEnd() != -1)
	{
		std::cout << "the stdinwriteend is " << cgi->getStdinWriteEnd() << std::endl;
		addToEpoll(_epollFd, cgi->getStdinWriteEnd(), EPOLLOUT);
		this->_pipeFd[cgi->getStdinWriteEnd()] = &client;
	}
	if (cgi->getStdoutReadEnd() != -1)
	{
		std::cout << "the stdoutreadend is " << cgi->getStdoutReadEnd() << std::endl;
		addToEpoll(_epollFd, cgi->getStdoutReadEnd(), EPOLLIN);
		this->_pipeFd[cgi->getStdoutReadEnd()] = &client;
	}
	return;
}

void Server::writeToPipe(Client &client, int stdinPipeWriteFd)
{
	Request *request = client.getRequest();
	size_t writtenBytes = 0;

	writtenBytes = write(stdinPipeWriteFd, request->getBody().c_str() + this->_bodyOffset, request->getBody().size() - this->_bodyOffset);
	if (writtenBytes < 0)
	{
		perror("Error: ");
		return ;
	}
	if (writtenBytes > 0)
	{
		std::cout << "appending the offset" << std::endl;
		this->_bodyOffset += writtenBytes;
		if (this->_bodyOffset >= request->getBody().size())
		{
			std::cout << "entered the case of the writtenbytrs == 0" << std::endl;
			this->_pipeFd.erase(stdinPipeWriteFd);
			delFromEpoll(_epollFd,stdinPipeWriteFd);
			return;
		}
	}
}

void Server::writeToClient(int fd)
{
	Client &client = *(_clients[fd]);
	std::string response = client.getResponseStr();
	send(fd, response.c_str(), response.size(), 0);
	std::cout << "response sent to the client :D" << std::endl;
	closeConnection(fd);
}

void Server::closeConnection(int fd)
{
	delFromEpoll(this->_epollFd, fd);
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
	return (this->_epollFd);
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
	if (this->_epollFd != -1)
	{
		close(this->_epollFd);
		this->_epollFd = -1;
	}
}
