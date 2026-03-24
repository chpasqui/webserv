#include "../include/server.hpp"
#include <string>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>


int handleClientsConnections(int listenFd)
{
	int					epollFd = -1;
	int					nbFd = -1;
	struct epoll_event	events[MAX_EVENT];
	struct epoll_event	event;

	event.events = EPOLLIN;
	event.data.fd = listenFd;
	epollFd = epoll_create(1);
	if (epollFd == -1)
	{
		perror("epoll_create");
		return (1);
	}

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, listenFd, &event) == -1)
	{
		close(epollFd);
		perror("epoll_ctl");
		return (1);
	}

	while (true)
	{
		nbFd = epoll_wait(epollFd, events, MAX_EVENT, 0); // 0 because we want it to be non blocking
		if (nbFd == -1)
			break ;

		for (int i = 0; i < nbFd; i++)
		{
			if (events[i].data.fd == listenFd)
			{
				std::cout << "new connection" << std::endl;
				struct sockaddr_in client_addr;
				(void) client_addr;
			}
		}
	}
	return (0);
}
