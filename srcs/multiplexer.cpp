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

#define BUFFER_SIZE 2

int handleClientsConnections(Server &server)
{
	int					nbFd = -1;
	struct epoll_event	events[MAX_EVENT]; // events est un tableau de fd comprenant une structure event par fd
	struct epoll_event	event; // une sturcture tampon singuliere qui est envoyee a epoll_ctl pour controler certains parametres (que l'on set)

	event.events = EPOLLIN;
	event.data.fd = listenFd;

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
		//check all fd time(); while loop if timeout => error 505
		for (int i = 0; i < nbFd; i++)
		{
			if (events[i].data.fd == listenFd) //New connection
				{
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int client_fd = accept(listenFd, (struct sockaddr *)&client_addr, &client_len);
					if (client_fd == -1)
					{
						perror("accept error");
						break;
					}
					// start measuring timestamp
					std::cout << "new connection" << std::endl; //debug
					event.events = EPOLLIN;
					event.data.fd = client_fd;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
					{
						perror("epoll_ctl: client_fd");
						close(client_fd);
					} 
			}
			else if (events[i].events & (EPOLLERR | EPOLLHUP))
			{
				printf("closed");
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
					perror("epoll_ctl_del");
				close(events[i].data.fd); //safe_close
			}
			else if (events[i].events & EPOLLIN)
			{
				//memset(&buffer, '\0', BUFFER_SIZE - 1);
				ssize_t bytesRead = recv(events[i].data.fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
				// update time
				if (bytesRead == 0) // in the case of the EOF signal at end of the client request
				{
					printf("closed\n");
					close(events[i].data.fd);
					continue;
				}
				else if (bytesRead == -1)
				{
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
						perror("epoll_ctl_del");
					close(events[i].data.fd);
					continue ;
				}
				buffer[bytesRead] = '\0';
				request += buffer; // add a result buffer "request" and concatenate what was read after epoll_wait call
				if (bytesRead < BUFFER_SIZE - 0) // end condition, no need to call read again, send for parsing of HTTP request 
				{
					std::cout << request << std::endl;
					Request request(buffer, events[i].data.fd); //TODO send completebuffer
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
						perror("epoll_ctl_del");
					close(events[i].data.fd); // dont forget to close the fd 
				}
			}
		}
		nbFd = 0;
	}
	return (0);
}

