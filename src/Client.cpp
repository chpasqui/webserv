#include "Client.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <unistd.h>

# define BUFFERSIZE 1024

Client::Client() :
	_clientFd(-1),
	_bytesRead(-1),
	_buffer(""),
	_config(NULL),
	_clientStatus(PARSING_REQUEST_LINE)
{
	std::cout << "Client default constructor called" << std::endl;
}

Client::Client(int fd, ServerConfig* config) :
	_clientFd(fd),
	_bytesRead(-1),
	_buffer(""), \
	_config(config),
	_clientStatus(PARSING_REQUEST_LINE)
{
	std::cout << "Client with fd" << fd << std::endl;
}

Client::Client(const Client& other) :
	_clientFd(other._clientFd),
	_bytesRead(other._bytesRead),
	_buffer(other._buffer),
	_config(other._config),
	_clientStatus(other._clientStatus) {}

void	Client::handleRequest()
{
	char buffer[1024];
	// IF timeout is not -1
		// TIMEOUT VALUE INITIAISED 
	this->_bytesRead = recv(_clientFd, buffer, sizeof(_buffer), MSG_DONTWAIT);
	if (_bytesRead == 0 || _bytesRead  == -1)
	{
		this->_clientStatus = CLOSEFD;
		return ;
	}
	if (this->_clientStatus == PARSING_REQUEST_LINE && this->_buffer.find("\r\n"))
	{
		//parseRequestLine(_buffer);
		std::cout << "entered parseRequestLine" << std::endl;
		this->_clientStatus = PARSING_HEADERS;
	}
	if (this->_clientStatus == PARSING_HEADERS && this->_buffer.find("\r\n\r\n"))
	{
		//partseHeaders(_buffer);
		std::cout << "entered parseHeaders" << std::endl;
		this->_clientStatus = PARSING_BODY;
	}
	if (this->_clientStatus == PARSING_BODY)
	{
		//timeout = 0
		//parseBody();
		std::cout << "entered parseBody" << std::endl;
		this->_clientStatus = COMPLETE;
	}
	// CHECK TIMEOUT because we never entered the conitions
	_buffer += std::string(buffer);
	return ;
}

int		Client::getClientFd() const
{
	return (this->_clientFd);
}

int		Client::getClientStatus() const
{
	return (this->_clientStatus);
}

Client::~Client() 
{
	std::cout << "Client destructor called" << std::endl;
}
