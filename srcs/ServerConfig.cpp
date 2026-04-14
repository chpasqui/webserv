#include "../include/ServerConfig.hpp"

#include <string>

ServerConfig::ServerConfig() :
	_autoindex(false),
	_port(0),
	_route(""),
	_allowedMethods("GET|POST|DELETE"),
	_locations(0),
	_index(0) //WARN: add the default "index.html" if this is empty after parsing
{
}

void ServerConfig::setPort(const int port)
{
	this->_port = port;
}

void ServerConfig::setRoute(const std::string& route) {
	this->_route = route;
}

int	ServerConfig::getPort(void)
{
	return (this->_port);
}

std::string ServerConfig::getRoute(void) const
{
	return (this->_route);
}

std::string ServerConfig::getAllowedMethods(void) const
{
	return (this->_allowedMethods);
}

bool ServerConfig::getAutoIndex(void) const
{
	return (this->_autoindex);
}

ServerConfig::~ServerConfig()
{
	
}
