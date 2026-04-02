#include "ServerConfig.hpp"

#include <string>

ServerConfig::ServerConfig() : _port(0), _route("")
{
}

void ServerConfig::setPort(const int port)
{
	this->_port = port;
}

void ServerConfig::setRoute(const std::string& route)
{
	this->_route = route;
}

int	ServerConfig::getPort(void)
{
	return (this->_port);
}

ServerConfig::~ServerConfig()
{
	
}