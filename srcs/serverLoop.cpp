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
			int fd = events[i].data.fd; // for example fd = 5 (the listening socket we added earlier)
			std::cout << "Server::run epollwait: current fd we are working on is " << fd << std::endl;
			if (_listenFds.count(fd))
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
				if (client_fd == -1)
				{
					perror("accept error");
					continue;
				}
				ServerConfig *config = _listenFds[fd]; // config 8080
				_clients.insert(std::make_pair(client_fd, new Client(client_fd, config, new Request()))); //_clients[6] = Client(fd of 6 created, address of ServerConfig for port 8080)
				//std::cout << "new client added to the _cliets map : " << _clients[fd]->getClientFd() << std::endl;
				addToEpoll(epollFd, client_fd, EPOLLIN); // watch fd = 6

			}
			else if (events[i].events & (EPOLLERR | EPOLLHUP))
			{
				delFromEpoll(epollFd, fd);
				close(fd);
				_clients.erase(fd);
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
					delFromEpoll(epollFd, fd);
					close(fd);
					std::cout << "Server::run in EPOLIN ,fd " << fd << " was closed because its status was CLOSEFD" << std::endl;
					delete _clients[fd];
					_clients.erase(fd);
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
				delFromEpoll(epollFd, fd);
				close(fd);
				std::cout << "After sending the response, fd " << fd << "is closed" << std::endl;
				delete(_clients[fd]);
				_clients.erase(fd);
			}
		}
	}
	std::cout << "end of the loop" << std::endl;
}
