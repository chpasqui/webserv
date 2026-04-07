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
		std::map<int, Client*> _clients;
	public:
		Server(std::vector<ServerConfig> serverConfigsVec);
		~Server();
		void	createEpollInstance(void);
		static void	addToEpoll(int epollFd, int fdToModify, uint32_t flag);
		static void	modifyEpoll(int epollFd, int fdToModify, uint32_t flag);
		static void	delFromEpoll(int epollFd, int fdToModify);
		int		getEpollFd(void) const;
		int		createListeningSocket(void);
		void	bindAndListen(int& fd, int port) const;
		void	run(void);
};




#endif