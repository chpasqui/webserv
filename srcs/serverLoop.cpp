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

/**
 * @brief I/O Multiplexing loop using the epoll API to monitor multiple file descriptors
 * to see if they are ready for I/O. The performance of epoll is much better than 
 * select or poll when monitoring large number of file descriptors. 
 * epoll will record specific fd in its interest list (listen fd is already 
 * added prior to this) in the queue, and maintains a lits of ready fds in the ready list.
 * 
 * The epollwait() system call returns items from the ready list associated with an epoll instance.
 * We use the epollctl() system call to add/modify/delete fd from the epoll instance.
 * 
 * Three different scenarios:
 * 1. We have a new connection -> goes to addNewConnection(fd) which will create a new
 * clientFd and add it to the monitored list in epoll
 * 2. One of the monitored fd has an event in whihc case it will go to readFromSocket (EPOLIN) 
 * where we will continuously loop back to the epollwait instance if there are n bytes to read,
 * parse it and handle POST GET DELETE / CGI POST GET
 * 3. ...or EPOLLOUT for writing to the client connecting socket (the response to the request)
 */
void	Server::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					nFd;

	while (!serverSignal) //WARN: Becareful with CGI signal too
	{
		//std::cout << "hi!" << std::endl;
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, 0);			// timeout is set to 0 tp perform non blocking checks 
		if (nFd == -1) break ;
		for (int i = 0; i < nFd; i++) {
			int fd = events[i].data.fd;
			if (_listenFds.count(fd)) { if (!addNewConnection(fd)) continue ; }
			else if (events[i].events & (EPOLLERR | EPOLLHUP)) removeConnection(fd);
			else if (events[i].events & EPOLLIN) readFromSocket(fd);		// data can be read
			else if(events[i].events & EPOLLOUT) writeToSocket(fd);			// data can be written
		}
	}
	std::cout << "signal to stop received" << std::endl;
}
