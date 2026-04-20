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
		addToEpoll(this->epollFd, clientFd, EPOLLIN);
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

/**
 * @brief the fd ready for EPOLLIN is sent received here, and they are coninously send to parseheader
 * until it is the entire request is treated (status changes to COMPLETE). Once the status
 * changes to COMPLETE, then the epollctl() call is called and modifies the flag for the 
 * monitored fd to EPOLLOUT. The server mulitplexer loop will then monitor EPOLLOUT. 
 * 
 * @param fd 
 */
void	Server::readFromSocket(int clientFd)
{
	Client& client = *(_clients[clientFd]); // _clients[6] will give us the Client class at clientFd = 6
	
	client.parseRequest();

	if (client.getParsingStatus() == CLOSEFD) {
		removeConnection(clientFd);
	}

	if (client.getParsingStatus() == COMPLETE) {
		client.buildResponse();
        if (!modifyEpoll(this->epollFd, fd, EPOLLOUT)){
            removeConnection(fd);
			return ;
		}
		// code for Response is here --> it will go to responseBuild which does the checks and then looks at the CGI
		std::cout << GREEN << "entered the COMPLETE function" << RESET << std::endl;
		Cgi cgi;
		client.setCgiPipeFd(cgi.handleCgi(client, "./www/cgi-bin/hello.py"));
		if (client.getCgiPipeFd() == -1) {
			removeConnection(client.getCgiPipeFd());
			return;
		}
		std::cout << "pipenreceived from piperead is " << client.getCgiPipeFd() << std::endl;
		// remove clientFd?
		addToEpoll(this->epollFd, client.getCgiPipeFd(), EPOLLIN);
		// if (!modifyEpoll(this->epollFd, clientFd, EPOLLOUT)){
		// 	removeConnection(clientFd);
		}
	}
}

void	Server::writeToSocket(int fd)
{
	// the response will be called here
	// read the pipe end
	Client& client = *(_clients[fd]);
    std::string response = client.getResponse();
    send(fd, response.c_str(), response.size(), 0);
	removeConnection(fd);
}

void	Server::removeConnection(int fd)
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
