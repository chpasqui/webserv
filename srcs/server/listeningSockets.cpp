#include "../../include/Server.hpp"

#include <string>
#include <stdexcept>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sstream>

/**
 * @brief Construct a new Server:: Server object by looping through 
 * the vector _configs and for each configuration, it will:
 * 1. create the epollfd
 * 2. loop through the vector of all the configs and create one listenfd for
 * each config, bind that listenfd to the specific port in the config,
 * update the map of _listenFds with a fd <==> configuration
 * 3. because fds are created incrementally, it will start at 5, 6, 7 etc.
 * (0 stdin, 1 stdout, 2 stderr, 3 epollfd, 4 listeningsocket, 5 listeningsocket)
 * so for the map call, we do _listenFds[4] = serverConfig[0]; then [5] <==> [1], etc.
 *
 */
void Server::setupListeningSockets(void) {
	createEpollInstance();
	std::string errorMsg;
	
	signal(SIGINT, Server::signalHandler);
	//signal(SIGQUIT, Server::signalHandler);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	
	for (size_t i = 0; i < _configs.size(); ++i)
	{
		int listenFd = openListeningSocket();
		bindAndListen(listenFd, _configs[i]->getPort());
		this->_listenFds[listenFd] = _configs[i];
		if (!addToEpoll(this->_epollFd, listenFd, EPOLLIN))
			throw(std::runtime_error(errorMsg));
	}
}

/**
 * @brief Creates a TCP listening socket, sets it to non blocking mode via fcntl()
 * and enables the SO_REUSEADDR to alllow the rebinding to another port in the 
 * TIME_WAIT state if ever we close the server and wnt to reconnect (avoids
 * )
 * 
 * @return the listenfd socket fd, ready to be bound to a port
 */
int Server::openListeningSocket() {
	int listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
	{
		throw std::runtime_error("Server: socket() failed.");
	}
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: fcntl() failed.");
	}
	int optval = 1;
	socklen_t optlen = sizeof(optval);
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: setsockopt() failed.");
	}
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
		std::stringstream msg;
		close(listenFd);
		msg << "Server: bind() failed on port ";
		msg << port;
		throw std::runtime_error(msg.str());
	}
	if (listen(listenFd, BACKLOG) == -1)
	{
		close(listenFd);
		throw std::runtime_error("Server: listen() failed.");
	}
	return ;
}
