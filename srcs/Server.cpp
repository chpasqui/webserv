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


#define BACKLOG 20
#define MAX_EVENTS 1024
#define TIMEOUT 10

// CONSTRUCTOR AND DESTRUCTOR

static int serverSignal = 0;
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
Server::Server(std::vector<ServerConfig*> serverConfigsVec) : _clients() {
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
	}
}

Server::~Server() {}


// SOCKET CREATION FOR SEVERS, BINDING

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
	std::cout << "Server::createEpollInstance epollFd " << epollFd << " is created" << std::endl; 
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

void	Server::addToEpoll(int epollFd, int fdToModify, uint32_t flag)
{
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fdToModify, &event) == -1)
	{
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::modifyEpoll(int epollFd, int fdToModify, uint32_t flag)
{

	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fdToModify, &event) == -1)
	{
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::delFromEpoll(int epollFd, int fdToModify)
{
	struct epoll_event	event;
	event.events = 0;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fdToModify, &event) == -1)
	{
		//WARN: if (errno == ENOENT) //don't close an inexistant file (Double close);
		//			return; 
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

// MAIN LOOP FOR RUNNING THE SERVER

void	Server::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					nFd;

	while (!serverSignal) //WARN: Becareful with CGI signal too
	{
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, 0);
		if (nFd == -1)
			break ;
		this->timeoutCheck(); //WARN:: We may need to move it and only perform a timeout test when a request is made. 
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
				_clients.insert(std::make_pair(client_fd, new Client(client_fd, config))); //_clients[6] = Client(fd of 6 created, address of ServerConfig for port 8080)
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

// TIME CHECKING UTILS

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

// SIGNAL HANDLING UTILS

void	Server::signalHandler(int signal) {
	if (signal == SIGINT)
		std::cout << "\nSIGINT signal detected, stopping server\n";
	else if (signal == SIGQUIT)
		std::cout << "\nSIGQUIT signal detected, stopping server\n";
	serverSignal = signal;
}


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
