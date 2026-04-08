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
	_clientStatus(PARSING_REQUEST_LINE),
	_request(NULL)
{
	std::cout << "Client default constructor called" << std::endl;
	time(&(this)->_timestamp);
}

Client::Client(int fd, ServerConfig* config, Request *request) :
	_clientFd(fd),
	_bytesRead(-1),
	_buffer(""), \
	_config(config),
	_clientStatus(PARSING_REQUEST_LINE),
	_request(request)
{
	std::cout << "Client with fd" << fd << " created" << std::endl;
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

bool Client::bodyRead(std::string& buffer){
	size_t pos = buffer.find("\r\n\r\n");
	buffer = buffer.substr(pos + 4);
	if (this->_request->getContentLength() == -1) {
		if (buffer.find("0\r\n\r\n") == std::string::npos)
			return false;
		return true;
	}
	if (buffer.size() < (size_t)_request->getContentLength())
		return false;
	return true;
}

void	Client::handleRequest()
{
	char buffer[1024]; //check garbage value when buffer size is small (10 for instance)
	// IF timeout is not -1
		// TIMEOUT VALUE INITIAISED 
	this->_bytesRead = recv(_clientFd, buffer, sizeof(buffer), MSG_DONTWAIT);
	if (_bytesRead == 0 || _bytesRead  == -1)
	{
		this->_clientStatus = CLOSEFD;
		return ;
	}
	_buffer += std::string(buffer, _bytesRead);
	if (this->_clientStatus == PARSING_REQUEST_LINE && this->_buffer.find("\r\n") != std::string::npos)
	{	
		std::cout << "parsing request line" << std::endl;
		//std::cout << _buffer << std::endl;
		if (!_request->parseRequestLine(_buffer)){
			this->_clientStatus	= COMPLETE;
			return ;
		}
		this->_clientStatus = PARSING_HEADERS;
	}
	if (this->_clientStatus == PARSING_HEADERS && this->_buffer.find("\r\n\r\n") != std::string::npos)
	{
		std::cout << "parsing header" << std::endl;
		if (!_request->parseHeader(_buffer)){
			this->_clientStatus	= COMPLETE;
			return ;
		}
		std::cout << "entered parseHeaders" << std::endl;
		this->_clientStatus = PARSING_BODY;
	}
	if (this->_clientStatus == PARSING_BODY && bodyRead(_buffer))
	{
		std::cout << "entered parseBody" << std::endl;
		if(!_request->parseBody(_buffer, 1024)){
			this->_clientStatus = COMPLETE;
			return;
		}
		this->_clientStatus = COMPLETE;
	}
	// CHECK TIMEOUT because we never entered the conitions
	std::cout << "end of function" << std::endl;
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
