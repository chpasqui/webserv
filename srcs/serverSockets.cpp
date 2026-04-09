#include "../include/Server.hpp"

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

/**
 * @brief Creates the epoll instance used to monitor all file descriptors (listening
 * sockets and client connections) throughout the server's lifetime. Sets the 
 * Server private attribute _epollFd to the returned file descriptor
 * 
 * @throws std::runtime_error if epoll_create() fails
 */
void	Server::createEpollInstance(void) {
	std::string errorMsg;

	this->epollFd = epoll_create(1);
	if (this->epollFd == -1)
	{
		errorMsg = std::string("Epoll_create() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
	std::cout << "Server::createEpollInstance epollFd " << epollFd << " is created" << std::endl; 
}

/**
 * @brief Creates a TCP listening socket, sets it to non blocking mode via fcntl()
 * and enables the SO_REUSEADDR to alllow the rebinding to another port in the 
 * TIME_WAIT state
 * 
 * @return the listenfd socket fd, ready to be bound to a port
 */
int Server::createListeningSocket() {
	int listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
	{
		throw std::runtime_error("Server: socket() failed.");
	}
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(epollFd);
		close(listenFd);
		throw std::runtime_error("Server: fcntl() failed.");
	}
	// close(listenFd);
	// throw std::runtime_error("FD TEST");
	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: setsockopt() failed.");
	}
	std::cout << "Server::createlisteningSocket listenFd " << listenFd << " is created" <<  std::endl;
	return listenFd;
}

/**
 * @brief Binds all the listening socket to the given port on all available 
 * network interfaces INADDR_ANY and marks them as passive via
 * the listen() syscall, allowing it to queue up to BACKLOG incoming connections 
 * 
 * @param listenFd listening socket to be bound, closed upon failure
 * @param port which is the serverConfigVec[i].getport() so that we bind the
 * listen fd to that specific port
 */
void	Server::bindAndListen(int &listenFd, int port) const {
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t addrlen = sizeof(sockaddr_in);

	if (bind(listenFd, (struct sockaddr*)&server_addr, addrlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: bind() failed.");
	}
	if (listen(listenFd, BACKLOG) == -1) // 1156 listen system call marks as passive socket
	{
		close(listenFd);
		throw std::runtime_error("Server: listen() failed.");
	}
	return ;
}

bool	Server::acceptNewConnection(int listeningFd)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientFd = accept(listeningFd, (struct sockaddr *)&client_addr, &client_len);
	if (clientFd == -1) // here we did a perror and not a try catch?
	{
		perror("accept error");
		return false;
	}
	ServerConfig *config = _listenFds[listeningFd];
	_clients.insert(std::make_pair(clientFd, new Client(clientFd, config, new Request())));
	addToEpoll(this->epollFd, clientFd, EPOLLIN); // addtoEPoll and all the epoll functions send back an exception which doesnt work i need to fix that
	return (true);
}
