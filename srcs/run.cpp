#include "../include/Server.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Client.hpp"

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <map>

#define MAX_EVENTS 1024


/**
 * @brief the Server() constructor does not need to receive anything, for now it is void because the server 
 * instance has access to the properties of the server class. 
 * 
 */
// Server::run(void )

// kind of the same logic but it will have to be adapted 
//  The fds have already been added in the constructor of the server. so we do not need to add them

	// then we enter the infinity loop of while 1 which will continuously run. 
	// epoll_wait call which will return the number of file descriptor which havee I/O activity on them

	// lets say a client connects on the port to a server, if the fd in the events structure = to a listening socket in the map
		// we accept the connection into a new socket represneint the conneciton with the client 
		// we go and search for the config based on the fd in the map <int, ServerConfig> config = _listentocongid[fd]
		// Once we have found the corresponding configuration to the fd, we give it to the client
		// we create a new client sending it the  config and we give it the address of config 
		// client[fd] = Client(fd, config)

	// otherwise there is an error 
		// treat the error
	
	// We have not received a listenfd, but we fall on a clientfd meaning there is i/o activity on it


	// if it is EPOLIN 
	// client = client[fd] (in the map strucutre) // for practical reaons easier to work with.
	// client.recv() // and logic according to that
	// if we receive all the message 
		// parse requesttline(client.buffer) && parsingStatus =  REQUESTLINE
		// parse header(buffer) && parsingStatus =  HEADER// request will be filled but the body is not read yet
		// definebody(client[fd].request) && parsingStatus =  BODY to find the content length and without or body, no body
			// internal 3 possibilities
		

	// if request completely read status && && parsingStatus =  COMPLETE
		// build response(instance of client) so that we have access to the configuration of the server and the request sent by client
		// change the flag to watch to EPOLLOUT in epoll_ctl()


	// else if it is EPOLLOUT then we do something
	// send(send the response on the client fd)
	// epoll_ctl delete
	// close()
	// client.buffer.empty();

void	Server::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					nFd;

	while (!this->_signal) //WARN: Becareful with CGI signal too
	{
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, 0);
		if (nFd == -1)
			break ;
		this->timeoutCheck(); //WARN:: We may need to move it and only perform a timeout test when a request is made. 
		for (int i = 0; i < nFd; i++)
		{
			int fd = events[i].data.fd; // for example fd = 5 (the listening socket we added earlier)
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
				Client *client = new Client(client_fd, config);
				_clients.insert(std::make_pair(client_fd, client)); //_clients[6] = Client(fd of 6 created, address of ServerConfig for port 8080)
				addToEpoll(client_fd, EPOLLIN); // watch fd = 6
			}
			else if (events[i].events & (EPOLLERR | EPOLLHUP))
			{
				delFromEpoll(fd);
				close(fd);
				_clients.erase(fd);
			}
			else if (events[i].events & EPOLLIN) // now in this case we would receive the fd = 6, its a client, and ready for EPOLIN
			{
				Client& client = *(this)->_clients[fd]; // _ciebts[6] will give us the Client class at fd = 6
				// send the llistenfd portentially
				if (client.recvSucceeded() == false)
				{
					delFromEpoll(fd);
					close(fd);
					//client.request.
					_clients.erase(fd);
					continue;
				}
				if (client.getParsingIsDone() == true)
				{
					modifyEpoll(fd, EPOLLOUT);
					//builresponse();
					// delFromEpoll(fd);
					// _clients.erase(fd);
					// close(events[i].data.fd); // dont forget to close the fd 
				}
			}
			else if(events[i].events & EPOLLOUT)
			{
				//send(response);
				delFromEpoll(fd);
				close(fd);
				_clients.erase(fd);
			}
		}
	}
	std::cout << "end of the loop" << std::endl;
}

