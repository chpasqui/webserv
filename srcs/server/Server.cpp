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
#include <wait.h>

#define MAX_EVENTS 1024
#define TIMEOUT 10
#define BUFFER_SIZE 1024

// CONSTRUCTOR AND DESTRUCTOR

int serverSignal = 0;

Server::Server() :	_epollFd(-1),
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
			else if (events[i].events & (EPOLLIN | EPOLLHUP))
			{
				if (this->_pipeFd.count(fd))
					readFromPipe(fd);
				else
					readFromClient(fd);
			}
			// else if (events[i].events & (EPOLLERR))
			// {
			// 	std::cout << "EPOLLERR case " << std::endl;
			// 	continue;
			// }
			else if (events[i].events & (EPOLLOUT)) //| EPOLLERR))
			{
				if (this->_pipeFd.count(fd))
				{
					std::cout << "the pipe we want to write to is" << fd << std::endl;
					Client *client = this->_pipeFd[fd];
					writeToPipe(*client, fd);
				}
				else
				{
					writeToClient(fd);
				}
			}
		}
	}
}


bool Server::acceptNewConnection(int listeningFd)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientFd = accept(listeningFd, (struct sockaddr *)&client_addr, &client_len);
	if (clientFd == -1)
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
		safeClose(clientFd);
		return false;
	}
	return (true);
}

void Server::closeConnection(Client *client)
{
	std::cout << "ENTERED THE CLOSE CONNECTION FUNCITON" << std::endl;
	if (client->getCgi())
	{
		if (client->getCgi()->getPipeWriteEnd() != -1)
			delFromEpoll(this->_epollFd, client->getCgi()->getPipeWriteEnd());
		if (client->getCgi()->getPipeReadEnd() != -1)
			delFromEpoll(this->_epollFd, client->getCgi()->getPipeReadEnd());
	}
	delFromEpoll(this->_epollFd, client->getClientFd());

	if (_clients.count(client->getClientFd()))
	{
		std::cout << "delte client" << std::endl;
		int fd = client->getClientFd(); // extract fd contained in client
		delete _clients[fd]; // delete the client pointer
		_clients.erase(fd); // remove the entry without access to client which has been deleted 
	}
}
// potentially rename to treatClientRequest
void Server::readFromClient(int clientFd)
{
	Client *client = _clients[clientFd];
	
	client->parseRequest();
	if (client->getParsingStatus() == CLOSEFD)
	{
		std::cout << "entered the CLOSEFD condition" << std::endl;
		closeConnection(client);
		return;
	}
	if (client->getParsingStatus() == COMPLETE)
	{
		bool isNew = false;
		Session &session = resolveSession(client->getRequest()->getHeader("cookie"), isNew);
		client->setResponse(session, isNew);
		client->getResponse()->build(*client->getRequest(), client->getServerConfig());
		if (client->getResponse()->getCgiStatus() == true)
		{
			launchCgi(*client);
		}
		else
		{
			if (!modifyEpoll(this->_epollFd, clientFd, EPOLLOUT))
			{
				std::cout << "entered the not modify epoll condition" << std::endl;
				closeConnection(client);
				return;
			}
		}
		std::cout << "the reading from the client is comlplete" << std::endl;
	}
}

void Server::launchCgi(Client &client)
{
	client.setCgi(client.getResponse(), client.getRequest());
	Cgi *cgi = client.getCgi();

	cgi->launch(*client.getServerConfig());

	if (errno)
	{
		closeConnection(&client);
		return ;
	}

	if (cgi->getPipeWriteEnd() != -1)
	{
		addToEpoll(_epollFd, cgi->getPipeWriteEnd(), EPOLLOUT);
		this->_pipeFd[cgi->getPipeWriteEnd()] = &client;
		std::cout << "pipe to be written to is " << cgi->getPipeWriteEnd() << std::endl;
	}

	if (cgi->getPipeReadEnd() != -1)
	{
		addToEpoll(_epollFd, cgi->getPipeReadEnd(), EPOLLIN);
		this->_pipeFd[cgi->getPipeReadEnd()] = &client;
		std::cout << "pipe to be read from " << cgi->getPipeReadEnd() << std::endl;

	}

	return;
}

void Server::readFromPipe(int pipeReadEnd)
{
	Client *client = this->_pipeFd[pipeReadEnd];
	Response *response = client->getResponse();

	char buffer[BUFFER_SIZE];
	ssize_t nbytes = 0;

	nbytes = read(pipeReadEnd, buffer, BUFFER_SIZE);
	if (nbytes > 0){
		client->setOutput(client->getOutput().append(buffer, nbytes));
		return;
	}
	else if (nbytes == 0)
	{
		std::cout << "entered the isdone case after the read" << std::endl;
		response->buildCgiResponse(client->getOutput());
		if (!modifyEpoll(_epollFd, client->getClientFd(), EPOLLOUT))
		{
			std::cout << "entered the case of read from pipe modify" << std::endl;
			closeConnection(client);
			return;
		}
		pid_t pid = client->getCgi()->getPid();
		int status = 0;
		waitpid(pid, &status, 0);
	}
	else {
		response->setError(502);
		if (!modifyEpoll(_epollFd, client->getClientFd(), EPOLLOUT))
		{
			closeConnection(client);
			return;
		}
	}
}

void Server::writeToPipe(Client &client, int pipeWriteEnd)
{
	Request *request = client.getRequest();
	ssize_t writtenBytes = 0;

	writtenBytes = write(pipeWriteEnd, request->getBody().c_str() + client.bodyoffset, request->getBody().size() - client.bodyoffset);
	if (writtenBytes < 0)
	{
		perror("Error: ");
		return ;
	}
	else if (writtenBytes > 0)
	{
		client.bodyoffset += writtenBytes;
		if (client.bodyoffset >= request->getContentLength())
		{
			closeConnection(&client);
			return;
		}
	}
}

void Server::writeToClient(int fd)
{
	Client *client = _clients[fd];
	std::string response = client->getResponseStr();
	ssize_t nbytes = send(fd, response.c_str() + client->sendoffset, response.size() - client->sendoffset, 0);
	if (nbytes < 0)
	{
		return ;
	}
	else if (nbytes  > 0)
	{
		client->sendoffset += nbytes;
		if (client->sendoffset >= response.size())
		{
			closeConnection(client);
			return ;
		}
	}
}


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
		delete (it->second);
	}
	_clients.clear();

	std::map<int, const ServerConfig*>::iterator listenIt = this->_listenFds.begin();
	for (; listenIt != this->_listenFds.end(); ++listenIt)
	{
		close(listenIt->first);
	}
	_listenFds.clear();
	safeClose(this->_epollFd);
}
