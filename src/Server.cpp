#include "Server.hpp"

#include <string>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <map>

#define BACKLOG 20
#define MAX_EVENTS 1024

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
Server::Server(std::vector<ServerConfig> serverConfigsVec) : _clients()
{
	createEpollInstance();
	std::string errorMsg;

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
void	Server::createEpollInstance(void)
{
	std::string errorMsg;

	this->epollFd = epoll_create(1);
	if (this->epollFd == -1)
	{
		errorMsg = std::string("Epoll_create() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

int Server::createListeningSocket()
{
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
void	Server::bindAndListen(int &listenFd, int port) const
{
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

void	Server::addToEpoll(int fd, uint32_t flag) const
{
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::modifyEpoll(int fd, uint32_t flag) const
{
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::delFromEpoll(int fd) const
{
	struct epoll_event	event;
	event.events = 0;
	event.data.fd = fd;
	if (epoll_ctl(this->epollFd, EPOLL_CTL_DEL, fd, &event) == -1)
	{
		close(this->epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

void	Server::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					nFd;

	while (true)
	{
		nFd = epoll_wait(this->epollFd, events, MAX_EVENTS, 0);
		if (nFd == -1)
			break ;
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
				_clients.insert(std::make_pair(client_fd, Client(client_fd, config))); //_clients[6] = Client(fd of 6 created, address of ServerConfig for port 8080)
				std::cout << "new connection to client, socket " << client_fd << "created" << std::endl;
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
				Client& client = _clients[fd]; // _ciebts[6] will give us the Client class at fd = 6
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


//VALIDER LA SEMANTIQUE DE LA REQUEST
// Step 1 : verify if both Transfer-Encoding ET Content-Length présents -> error 400
// Step 2: if HTTP/1.1 et "host" absent --> error 400
// Step 3: si method == POST et ni "content-length" ni "transfer-encoding" --> error 411


// // faire un isValid au lieu decrire partout == 200 bool isValid() const { return _statusCode == 200; }

// //ATTENTION POUR BOUCLE EPOLL:
// fd = append()
// if (STATE == PARSING_REQUEST_LINE buffer.find("\r\n"))
// {
//    	parseRequestLine(fd);
//     STATE = PARSING_HEADERS;
// }

// if (STATE == PARSING_HEADERS && buffer.find("\r\n\r\n"))
// {
//     if (request.isValid) // au lieu de statusCode==200 par exemple
//         parseHeaders(buffer);
// 	  // need to validation sémantique
//     if (statusCode == 200)
//         validateRequest(maxbodysize);
    
//     STATE = PARSING_BODY;
// }

// if (STATE == PARSING_BODY)
// {
//     if (statusCode == 200)
//         defineBody(buffer);
//     STATE = COMPLETE;
// }

// if (STATE == COMPLETE)
// {
//     response = buildResponse(request, serverConfig);
//     // et on switch to EPOLLOUT
// }