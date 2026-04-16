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
		//std::cout << "hi!" << std::endl;
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, -1);
		if (nFd == -1) break ;
		for (int i = 0; i < nFd; i++) {
			int fd = events[i].data.fd;
			//std::cout << "Server::run epollwait: current fd we are working on is " << fd << std::endl;
			if (_listenFds.count(fd)) { if (!addNewConnection(fd)) continue ; }
			else if (events[i].events & (EPOLLERR | EPOLLHUP)) removeConnection(fd);
			else if (events[i].events & EPOLLIN) readFromSocket(fd);
			else if(events[i].events & EPOLLOUT) writeToSocket(fd);
		}
	}
	std::cout << "signal to stop received" << std::endl;
}
