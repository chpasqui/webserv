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

Server::Server() : _epollFd(-1),
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
			else if (events[i].events & (EPOLLERR))
			{
				std::cout << "error event is " << events[i].events << std::endl;
				closeConnection(fd);
			}
			else if (events[i].events & (EPOLLIN | EPOLLHUP))
			{
				if (this->_pipeFd.count(fd))
				{
					readFromPipe(fd);
				}
				else
					readFromClient(fd);
			}
			else if (events[i].events & EPOLLOUT)
			{
				if (this->_pipeFd.count(fd))
				{
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
	std::cout << "signal to stop received" << std::endl;
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
		closeConnection(clientFd);
		return false;
	}
	return (true);
}

// potentially rename to treatClientRequest
void Server::readFromClient(int clientFd)
{
	Client &client = *(_clients[clientFd]);

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
		if (client.getResponse()->getCgiStatus() == true)
		{
			std::cout << "launch cgi entered" << std::endl;
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

void Server::launchCgi(Client &client)
{
	client.setCgi(client.getResponse(), client.getRequest());
	Cgi *cgi = client.getCgi();
	cgi->launch(*client.getServerConfig());
	if (errno)
	{
		closeConnection(client.getClientFd());
		return ;
	}
	if (cgi->getStdinWriteEnd() != -1)
	{
		std::cout << "GET METHPD ENTERED ALERT" << std::endl;
		addToEpoll(_epollFd, cgi->getStdinWriteEnd(), EPOLLOUT);
		this->_pipeFd[cgi->getStdinWriteEnd()] = &client;
		std::cout << "pipe to be written to is " << cgi->getStdinWriteEnd() << std::endl;
	}
	if (cgi->getStdoutReadEnd() != -1)
	{
		addToEpoll(_epollFd, cgi->getStdoutReadEnd(), EPOLLIN);
		this->_pipeFd[cgi->getStdoutReadEnd()] = &client;
		std::cout << "pipe to be read from " << cgi->getStdoutReadEnd() << std::endl;

	}
	std::cout << "the stinread end should contain a number " <<  cgi->getStdoutReadEnd() << " and the stdinpipe should be -1: VALUE " <<   cgi->getStdoutReadEnd() << std::endl;
	return;
}

void Server::readFromPipe(int stdoutPipeReadFd)
{
	Client *client = this->_pipeFd[stdoutPipeReadFd];
	char buffer[BUFFER_SIZE];
	ssize_t nbytes = 0;
	nbytes = read(stdoutPipeReadFd, buffer, BUFFER_SIZE);
	if (nbytes > 0){
		client->setOutput(client->getOutput().append(buffer, nbytes));
		return;
	}
	else if (nbytes == 0)
	{
		std::cout << "entered the isdone case after the read" << std::endl;
		client->getResponse()->buildCgiResponse(client->getOutput());
		if (!modifyEpoll(_epollFd, client->getClientFd(), EPOLLOUT))
		{
			closeConnection(client->getClientFd());
			return;
		}
		pid_t pid = client->getCgi()->getPid();
		int status = 0;
		waitpid(pid, &status, 0);
		// if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0)))
		// {
		// 	client->getResponse().
		// close the connection in this case
		// }
	}
	else {
		client->getResponse()->setError(502);
		if (!modifyEpoll(_epollFd, client->getClientFd(), EPOLLOUT))
		{
			closeConnection(client->getClientFd());
			return;
		}
	}
	delFromEpoll(_epollFd, stdoutPipeReadFd);
	safeClose(stdoutPipeReadFd);
	_pipeFd.erase(stdoutPipeReadFd);
}

void Server::writeToPipe(Client &client, int stdinPipeWriteFd)
{
	Request *request = client.getRequest();
	size_t writtenBytes = 0;

	writtenBytes = write(stdinPipeWriteFd, request->getBody().c_str() + client.bodyoffset, request->getBody().size() - client.bodyoffset);
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
			this->_pipeFd.erase(stdinPipeWriteFd);
			delFromEpoll(_epollFd,stdinPipeWriteFd);
			safeClose(stdinPipeWriteFd);
			return;
		}
	}
}

void Server::writeToClient(int fd)
{
	Client &client = *(_clients[fd]);
	std::string response = client.getResponseStr();
	// Just like the write call, then i will need ot use the offset --> I need to keep the offset in client 
	ssize_t nbytes = send(fd, response.c_str() + client.sendoffset, response.size() - client.sendoffset, 0);
	if (nbytes < 0)
	{
		// handle the error message
		// if the send system call fails, does that mean that we need to close evryhting and come back to the main loop?
		return ;
	}
	else if (nbytes  > 0)
	{
		client.sendoffset += nbytes;
		if (client.sendoffset >= response.size())
		{
			closeConnection(fd);
			return ;
		}
	}
}

void Server::closeConnection(int fd)
{
	delFromEpoll(this->_epollFd, fd);
	safeClose(fd);
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
		safeClose(it->first);
	}
	_clients.clear();

	std::map<int, const ServerConfig *>::iterator secondIt = this->_listenFds.begin();
	for (; secondIt != this->_listenFds.end(); ++secondIt)
	{
		safeClose(secondIt->first);
	}
	_listenFds.clear();
	safeClose(this->_epollFd);
}
