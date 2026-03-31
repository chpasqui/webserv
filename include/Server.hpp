#ifndef SERVER_HPP
# define SERVER_HPP
# define BACKLOG 5
# define PORT 8080

#include <string>
#include <map>

#include "Client.hpp"

class Server
{
	public:
		Server();
		Server(const Server& other);
		~Server();
		void	createListeningSocket();
		void	bindAndListen();
		int		getListenFd() const;
	private:
		int						_listenFd;
		int						_port;
		std::string				_root;
		std::map<Client, int>	_clients;
};

void	setListeningSocketOptions(int& _listenFd);
int		handleClientsConnections(int listenFd);

#endif
