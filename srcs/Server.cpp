#include "../include/Server.hpp"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <csignal>
#include <signal.h>


#define BACKLOG 20
#define MAXEVENT 1024
#define TIMEOUT 10

/**
 * @brief Construct a new Server:: Server object by looping through 
 * the @param serverConfigsVec and for each configuration, it will:
 * 1. create the epollfd
 * 2. loop through the vector of all the configs and create one listenfd for
 * each config, bind that listenfd to the specific port in the config,
 * update the map of _listenFds with a fd <==> config file
 * 3. because fds are created incrementally, it will start at 5, 6, 7 etc.
 * (0 stdin, 1 stdout, 2 stderr, 3 epollfd, 4 listeningsocket, 5 listeningsocket)
 * so for the map call, we do _listenFds[4] = serverConfig[0]; then [5] <==> [1]
 * 
 * @param serverConfigsVec (loop through)
 */
Server::Server(std::vector<ServerConfig> serverConfigsVec) : _clients(), _signal(0) {
	createEpollInstance();
	std::string errorMsg;

	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);

	Server::_instance = this;
	for (size_t i = 0; i < serverConfigsVec.size(); ++i)
	{
		int listenFd = createListeningSocket();
		bindAndListen(listenFd, serverConfigsVec[i].getPort());
		this->_listenFds[listenFd] = &serverConfigsVec[i];
		addToEpoll(listenFd, EPOLLIN);
	}
}

/**
 * @brief Create a new listeningSocket and set its options to non blocking. 
 * It sets the private attribute epollFd to the single fd (3)
 */
void	Server::createEpollInstance(void) {
	std::string errorMsg;

	this->epollFd = epoll_create(1);
	if (this->epollFd == -1)
	{
		errorMsg = std::string("Epoll_create() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
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

Server::~Server() {}

void	Server::addToEpoll(int fd, uint32_t flag) const {
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl_add() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}


void	Server::modifyEpoll(int fd, uint32_t flag) const {
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl_mod() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::delFromEpoll(int fd) const {
	struct epoll_event	event;
	event.events = 0;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_DEL, fd, &event) == -1)
	{
		//FIX: if (errno == ENOENT)
		//			return; 
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl_del() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::timeoutCheck(void) {
	time_t	currentTime;

	time(&currentTime);
	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (std::difftime(currentTime, it->second->getTimestamp()) == TIMEOUT)
		{
			std::cout << "Timeout on socket " << it->first << std::endl;
			//buildReponse();
			//this->modifyEpoll(it->first, EPOLLOUT);
			//sendResponse();
			this->_clients.erase(it->first);
		}
	}
}

void	Server::signalHandler(int signal) {
	if (signal == SIGINT)
		std::cout << "\nSIGINT signal detected, stopping server\n";
	else if (signal == SIGQUIT)
		std::cout << "\nSIGQUIT signal detected, stopping server\n";
	Server::_instance->_signal = signal;
}

/**
 * @brief there is only one Server class and it will hve one fd for the epollfd
 * it will also contain the map for the rest.
 * 
 */
//Server(vector<int, ServerConfig> serverConfigs)
//{
	// create the epoll instance and keep it as the instance of that server
	// this solves the question of the 1 epoll instance for everyone
	// 	std::string	errorMsg;

	// errno = 0;
	// this->_epollFd = epoll_create(1);
	// if (this->_epollFd == -1)
	// {
	// 	errorMsg = std::string("Epoll_create error: ") + strerror(errno);
	// 	throw (std::runtime_error(errorMsg));
	// }
	// return ;


	// Server CONFIGS (so thr main server which will bind and create the listening sockets for the corresponding ports and conf)
	// Loop over the config file 
		// FIRST LOOP
		// fd = createListening socket; (creates the new fd = 5)
		// bind(fd, configport); bindlisteningsocket to the port in the config (fd 5 is bound to port 8080)
		// listen(fd, BACKLOG); call listen(fd) for fd 5
		// _listenFdToConfig = &ServerConfig; // fd 5 belongs to this config
		// epoll_ctl(ADD the listening socket fd = 5)
		
		//SECOND LOOP
		// fd = createListening socket; (creates the new fd = 6)
		// bindlisteningsock to the port in the config (fd 6 is bound to port 9090)
		// call listen(fd) for fd 6
		// _listenfdToCorrespondingConfig = & serverConfig; // fd 6 belongs to this config
		// epoll_ctl(add the listening socket fd = 6 )
//}

Server *Server::_instance = 0; //OPTIM: if singleton class, use getInstance() fonction

