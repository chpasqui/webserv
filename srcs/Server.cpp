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
#include "../include/color.hpp"


#define MAX_EVENTS 1024
#define TIMEOUT 10

int serverSignal = 0;

Server::Server() : 
		epollFd(-1),
		_listenFds(),
		_clients()
{
	std::cout << "Server default constructor called" << std::endl;
}

/**
 * @brief Construct a new Server:: Server object by looping through 
 * the @param serverConfigsVec and for each configuration, it will:
 * 1. create the epollfd
 * 2. loop through the vector of all the configs and create one listenfd for
 * each config, bind that listenfd to the specific port in the config,
 * update the map of _listenFds with a fd <==> configuration
 * 3. because fds are created incrementally, it will start at 5, 6, 7 etc.
 * (0 stdin, 1 stdout, 2 stderr, 3 epollfd, 4 listeningsocket, 5 listeningsocket)
 * so for the map call, we do _listenFds[4] = serverConfig[0]; then [5] <==> [1], etc.
 * 
 * @param serverConfigsVec (loop through)
 */
void Server::configureVirtualServers(std::vector<ServerConfig>& serverConfigsVec) {
	
	std::cout << "\n SETTING UP THE SERVERS \n" << std::endl;
	createEpollInstance();
	std::string errorMsg;
	
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
	
	for (size_t i = 0; i < serverConfigsVec.size(); ++i)
	{
		int listenFd = createListeningSocket();
		bindAndListen(listenFd, serverConfigsVec[i]->getPort());
		this->_listenFds[listenFd] = serverConfigsVec[i];
		addToEpoll(epollFd, listenFd, EPOLLIN);
		if (i == 1)
		{
			close(this->epollFd);
			std::map<int, ServerConfig*>::iterator it = this->_listenFds.begin();
			for ( ; it != this->_listenFds.end(); ++it)
			{
				std::cout << "fd " << it->first << " is closed" << std::endl;
				close(it->first);
			}
			std::string errorMsg = std::string("test memory closing") + strerror(errno);
			throw (std::runtime_error(errorMsg));
		}
	}
}

Server::~Server() {
	std::cout << "Server destructor called" << std::endl;
	std::map<int, Client*>::iterator it = this->_clients.begin();
	for ( ; it != this->_clients.end(); ++it)
	{
		close(it->first);
		//delete(it->second);
	}
	_clients.clear();

	std::map<int, ServerConfig*>::iterator secondIt = this->_listenFds.begin();
	for ( ; secondIt != this->_listenFds.end(); ++secondIt)
	{
		close(secondIt->first);
		//delete(secondIt->second);
	}
	_listenFds.clear();
	if (this->epollFd != -1)
	{
		std::cout << "closing epollFd" << epollFd << std::endl;
		close(this->epollFd);
		this->epollFd = -1;
	}
	std::cout << "done destroying the server" << std::endl;
}

int Server::createListeningSocket() {
	int listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
		throw std::runtime_error("Server: socket() failed.");

	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Server: fcntl() failed.");

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
 * @brief binds the listening fd to the corresponding port in the
 * configuration file. The port number is used by the kernel to match
 * an incoming packet to a certain process's socket descriptor.et it to listen
 * to incoming connecitons on a specific port. Backllog says how many
 * connections are allower to queue up.
 * 
 * @param listenFd is the one created in the main constructor 
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

	std::cout << C_PURPLE << "DEBUG: PORT IS " << port << RESET << std::endl;

	if (bind(listenFd, (struct sockaddr*)&server_addr, addrlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: bind() failed.");
	}

	if (listen(listenFd, BACKLOG) == -1) // 1156 listen system call marks as passive socket
	{
		close(listenFd); //safe close
		throw std::runtime_error("Server: listen() failed.");
	}
	return ;
}

// EPOLL HELPER FUNCTIONS

int		Server::getEpollFd(void) const
{
	return (this->epollFd);
}
