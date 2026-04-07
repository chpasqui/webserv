#include "../include/Client.hpp"
#include "../include/Client.hpp"
#include "../include/ServerConfig.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unistd.h>
#include <iostream>

# define BUFFERSIZE 1024

Client::Client() :
	_clientFd(-1),
	_bytesRead(-1),
	_buffer(""),
	_config(NULL),
	_clientStatus(PARSING_REQUEST_LINE)
{
	std::cout << "Client default constructor called" << std::endl;
	time(&(this)->_timestamp);
}

Client::Client(int fd, ServerConfig* config) :
	_clientFd(fd),
	_bytesRead(-1),
	_buffer(""), \
	_config(config),
	_clientStatus(PARSING_REQUEST_LINE)
{
	std::cout << "Client with fd" << fd << std::endl;
	time(&(this)->_timestamp);
}

Client::Client(const Client& other) :
	_clientFd(other._clientFd),
	_bytesRead(other._bytesRead),
	_buffer(other._buffer),
	_config(other._config),
	_clientStatus(other._clientStatus),
	_timestamp(other._timestamp)
{}

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

time_t	Client::getTimestamp(void) const
{
	return (this->_timestamp);
}

// void	Client::requestComplete()
// {
// 	size_t pos = this->_buffer.find("\r\n\r\n");
// 	if (pos == std::string::npos) // npos indicates the not found state
// 	{
// 		return;
// 	}


// 	// getline with the delimiter

// 	// if (_ParsingStatus == REQUESTLINE)
// 	// 	parseRequestLine(); // set to header
	
// 	// if (_ParsingStatus = HEADER)
// 	// 	parseHEadeers(); // set to body

// 	// if (_ParsingStatus == BODY)
// 	// 	defineBody(); // set bool _isdone
// }

int		Client::getClientStatus() const
{
	return (this->_clientStatus);
}

Client::~Client() 
{
	std::cout << "Client destructor called" << std::endl;
}
