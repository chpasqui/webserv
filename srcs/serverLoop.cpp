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

void	Server::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					nFd;

	while (!serverSignal) //WARN: Becareful with CGI signal too
	{
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, 0);
		if (nFd == -1)
			break ;
		//this->timeoutCheck(); //WARN:: We may need to move it and only perform a timeout test when a request is made. 
		for (int i = 0; i < nFd; i++)
		{
			int fd = events[i].data.fd;
			std::cout << "Server::run epollwait: current fd we are working on is " << fd << std::endl;
			if (_listenFds.count(fd))
			{
				if (!acceptNewConnection(fd)) continue ; // 
			}
			else if (events[i].events & (EPOLLERR | EPOLLHUP))
			{
				closeAndRemoveFromEpoll(fd);
			}
			// Now the fd is the clientfd and it had activity so 6
			else if (events[i].events & EPOLLIN) // now in this case we would receive the fd = 6, its a client, and ready for EPOLIN
			{
				std::cout << "Server::run inside EPOLIN : existing connection " << fd << " is sending a request." << std::endl;
				//timeout
				Client& client = *(_clients[fd]); // _clients[6] will give us the Client class at fd = 6
				
				client.handleRequest();
				if (client.getClientStatus() == CLOSEFD)
				{
					closeAndRemoveFromEpoll(fd);
				}
				if (client.getClientStatus() == COMPLETE)
				{
					std::cout << "building response..." << std::endl;
					///buildResponse()
					modifyEpoll(epollFd, fd, EPOLLOUT);
				} 
			}
			else if(events[i].events & EPOLLOUT)
			{
				std::cout << "sending response now :)" << std::endl;
				//send(response);
				closeAndRemoveFromEpoll(fd);
			}
		}
	}
	std::cout << "end of the loop" << std::endl;
}

void	Server::closeAndRemoveFromEpoll(int fd)
{
	delFromEpoll(epollFd, fd);
	if (close(fd) == -1)
		std::cerr << "Close failed" << std::endl; // check if we mannage this differently or not
	delete _clients[fd];
	_clients.erase(fd);
}