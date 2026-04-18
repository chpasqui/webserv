#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include <vector>
#include <stdint.h>

#include "ServerConfig.hpp"
#include "Client.hpp"

extern int serverSignal;

// Client lives in server
// ServerConfig lives in Client
// Server lives in main
//server outlives serverconfig so it can have a pointer to serverconfig and it will delete it when closing


#define MAX_EVENTS 1024
#define TIMEOUT 10
#define BACKLOG 20


class Server
{
	public:
		Server(char **env);
		void setupListeningSockets(const std::vector<ServerConfig>& serverConfigsVec);
		~Server();
		void	createEpollInstance(void);
		int		getEpollFd(void) const;
		int		openListeningSocket(void);
		void	bindAndListen(int& fd, int port) const;
		void	run(void);
		bool	addNewConnection(int listeningFd);
		void	readFromSocket(int fd);
		void	writeToSocket(int fd);
		void	timeoutCheck(void);
		
		void	removeConnection(int fd);
		static void	addToInterestList(int epollFd, int fdToModify, uint32_t flag);
		static bool	modifyEpoll(int epollFd, int fdToModify, uint32_t flag);
		static void	removeFromInterestList(int epollFd, int fdToModify);
		static void	signalHandler(int signal);


	private:
		int	epollFd;
		char **env;
		std::map<int, const ServerConfig*> _listenFds; // if it wasnt a pointer, server would own another copy of the serverconfig which is wasteful
		std::map<int, Client*> _clients;
};

#endif