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
	int					epoll_fd = -1;
	int					nbFd = -1;
	struct epoll_event	events[MAX_EVENT]; // events est un tableau de fd comprenant une structure event par fd
	struct epoll_event	event; // une sturcture tampon singuliere qui est envoyee a epoll_ctl pour controler certains parametres (que l'on set)

	event.events = EPOLLIN;
	event.data.fd = listenFd;
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		perror("epoll_create");
		return (1);
	}

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenFd, &event) == -1)
	{
		close(epoll_fd);
		perror("epoll_ctl");
		return (1);
	}

	while (true)
	{
		nbFd = epoll_wait(epoll_fd, events, MAX_EVENT, 0); // 0 because we want it to be non blocking
		if (nbFd == -1)
			break ;

		for (int i = 0; i < nbFd; i++)
		{
			if (events[i].data.fd == listenFd)
			{
				while (1)
				{
					std::cout << "new connection" << std::endl;
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int client_fd = accept(listenFd, (struct sockaddr *)&client_addr, &client_len);
					if (client_fd == -1)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break ;
						else
						{
							perror("accept error");
							break;
						}
					}
					event.events = EPOLLIN | EPOLLET;
					event.data.fd = client_fd;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
					{
						perror("epoll_ctl: client_fd");
						close(client_fd);
					} 
				}
			}
		}
	}
	return (0);
}
