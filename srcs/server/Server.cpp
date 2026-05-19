#include "../../include/Server.hpp"
#include "../../include/Session.hpp"
#include "../../include/Cgi.hpp"
#include "../../include/statusCode.hpp"

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
		nFd = epoll_wait(this->_epollFd, events, MAX_EVENTS, 1000);
		if (nFd == -1 || errno == EINTR)
			continue ;
		//check if nFd equals -1 if signals arrived
		//need to change the timeout of epoll_wait for checks 
		timeoutCheck();
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
				else if (this->_clients.count(fd))
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
					writeToPipe(fd);
				else if (this->_clients.count(fd))
					writeToClient(fd);
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
	if (!addToEpoll(this->_epollFd, clientFd, EPOLLIN)){
		safeClose(clientFd);
		return false;
	}
	// add the multiple serverconfigs in one fd and map with the host name (multiple config for one port)
	const ServerConfig *config = _listenFds[listeningFd];
	_clients.insert(std::make_pair(clientFd, new Client(clientFd, config)));
	return (true);
}

/**
 * @brief Clean up function for if the connection encounters an error it will:
 * - remove all the pipes form the epoll interest list
 * - delete the instance of the client, which will call the destructors in
 * CGI, Request and Response and close client fd. The pipes are closed inside
 * the CGI destructor (chain of destruction).
 * 
 * The check for if they are in the array is necessary as this would be in the case where the 
 * read and write did not function.
 * 
 * @param client as it has access to all the fd that I need (access to the pipe map and the clients map)
 */
void Server::closeConnection(Client *client)
{
	if (client->getCgi())
	{	if (client->getCgi()->getPid() > 0)
			kill(client->getCgi()->getPid(), SIGKILL);
		// this is a precautionary measure, in case the read and write have failed, then i need to double check that they are rempoved from epoll
		if (client->getCgi()->getPipeWriteEnd() != -1 && _pipeFd.count(client->getCgi()->getPipeWriteEnd()))
			delFromEpoll(this->_epollFd, client->getCgi()->getPipeWriteEnd());
		if (client->getCgi()->getPipeReadEnd() != -1 && _pipeFd.count(client->getCgi()->getPipeReadEnd()))
			delFromEpoll(this->_epollFd, client->getCgi()->getPipeReadEnd());
	}
	
	int fd = client->getClientFd();// extract fd contained in client
	if (_clients.count(client->getClientFd()) && _clients[fd] == client)
	{
		delFromEpoll(this->_epollFd, client->getClientFd());
		delete _clients[fd]; // delete the client pointer
		_clients.erase(fd); // remove the entry without access to client which has been deleted
	}
	else
		delete client;
}

/**
 * @brief First function called to read and parse the request from the newly accepted function. 
 * It parses the request and identifies if
 * - it is a CGI in which case it launches the CGI 
 * - it is not a CGI, meaning it will call epol_ctl to change to EPOLLOUt so that we can
 * treat the normal POST/GET/DELETE requests. 
 * 
 * @param clientFd 
 */
void Server::readFromClient(int clientFd)
{
	Client *client = _clients[clientFd];
	updateTimeStamp(client);
	client->parseRequest();
	if (client->getParsingStatus() == CLOSEFD)
	{
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
			if (launchCgi(*client))
				delFromEpoll(this->_epollFd, clientFd);
		}
		else
		{
			if (!modifyEpoll(this->_epollFd, clientFd, EPOLLOUT))
			{
				closeConnection(client);
				return;
			}
		}
	}
}

/**
 * @brief Opens the Common Gateway Interface (forks a child process for the script) and 
 * returns the pipe ends:
 * - only the PipeReadEnd in the case of a "GET" request (just needs to read the child output)
 * - both the PipeWriteEnd and the PipeReadEnd in the case of a "POST" requerst (needs
 * to send the body to the cgi and then read from the cgi once the pipe is ready to be read
 * from (EPOLLIN))
 * 
 * @param client 
 */
bool Server::launchCgi(Client &client)
{
	client.setCgi(client.getResponse(), client.getRequest(), this);
	Cgi *cgi = client.getCgi();
	
	errno = 0;
	cgi->launch(*client.getServerConfig());
	if (cgi->getResponse()->getStatusCode())
	{
		modifyEpoll(this->_epollFd, client.getClientFd(), EPOLLOUT);
		return false;
	}
	if (cgi->getPipeWriteEnd() != -1)
	{
		if (!addToEpoll(_epollFd, cgi->getPipeWriteEnd(), EPOLLOUT)){
			closeConnection(&client);
			return false;
		}
		this->_pipeFd[cgi->getPipeWriteEnd()] = &client;
	}

	if (cgi->getPipeReadEnd() != -1)
	{
		if (!addToEpoll(_epollFd, cgi->getPipeReadEnd(), EPOLLIN)){
			closeConnection(&client);
			return false;
		}
		this->_pipeFd[cgi->getPipeReadEnd()] = &client;
	}
	return true;
}

/**
 * @brief In the case of a "POST" request, the partent process needs to send the 
 * 
 * @param client 
 * @param pipeWriteEnd 
 */
void Server::writeToPipe(int pipeWriteEnd)
{

	Client *client = this->_pipeFd[pipeWriteEnd];
	Request *request = client->getRequest();
	ssize_t writtenBytes = 0;

	writtenBytes = write(pipeWriteEnd, request->getBody().c_str() + client->bodyoffset, request->getBody().size() - client->bodyoffset);
	if (writtenBytes < 0)
	{
		perror("Error: ");
		return ;
	}
	else if (writtenBytes > 0)
	{
		updateTimeStamp(client);
		client->bodyoffset += writtenBytes;
		if (client->bodyoffset >= request->getContentLength())
		{
			delFromEpoll(this->_epollFd, pipeWriteEnd);
			this->_pipeFd.erase(pipeWriteEnd);
			client->getCgi()->closePipeWriteEnd();//pour close le PipeWriteEnd de la classe Cgi (pas la variable locale)
			return;
		}
	}
}

/**
 * @brief Common path for all CGI requests, parent process reads from the pipe
 * for the CGI output in chunks and once all bytes are read, it will 
 * call buildCgiResponse() which will parse and create the headers
 * for the client output.
 * 
 * Once the function has finished reading, it rmoves the connection from epoll,
 * erases it from the pipefd map, and close it. It does not call close connection
 * else it would close both the read and write pipe ends which would cause 
 * a broken pipe error.
 * 
 * @param pipeReadEnd 
 */
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
		delFromEpoll(this->_epollFd, pipeReadEnd);
		this->_pipeFd.erase(pipeReadEnd);
		client->getCgi()->closePipeReadEnd();//pour close le PipeReadEnd de la classe Cgi (pas la variable locale)

		response->buildCgiResponse(client->getOutput());
	}
	else
		client->getResponse()->setError(BAD_GATEWAY, "readFromPipe: read failed");
	if (!addToEpoll(_epollFd, client->getClientFd(), EPOLLOUT))
	{
		closeConnection(client);
		return;
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
		updateTimeStamp(client);
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
		delete (listenIt->second);
	}
	_listenFds.clear();
	safeClose(this->_epollFd);
}
