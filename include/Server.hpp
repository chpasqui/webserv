#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include <vector>
#include <stdint.h>

#include "ServerConfig.hpp"
#include "Client.hpp"



// Client lives in server
// ServerConfig lives in Client
// Server lives in main
//server outlives serverconfig so it can have a pointer to serverconfig and it will delete it when closing
class Server
{
	private:
		int epollFd;
		std::map<int, ServerConfig*> _listenFds; // if it wasnt a pointer, server would own another copy of the serverconfig which is wasteful
		std::map<int, Client> _clients;
	public:
		Server(std::vector<ServerConfig> serverConfigsVec);
		~Server();
		void	createEpollInstance(void);
		void	addToEpoll(int fd, uint32_t flag);
		void	modifyEpoll(int fd, uint32_t flag);
		void	delFromEpoll(int fd);

		int		createListeningSocket(void);
		void	bindAndListen(int& fd, int port);
		void	run(void);
};




#endif