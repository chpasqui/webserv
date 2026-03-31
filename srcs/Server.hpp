#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <map>

#include "Client.hpp"

class Server
{
private:
	int						_listenFd;
	int						_port;
	std::string				_root;
	std::map<Client, int>	_clients;

public:
	Server();
	Server(const Server& other);
	~Server();
};

#endif
