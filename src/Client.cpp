#include "Client.hpp"
#include "ServerConfig.hpp"
#include <sys/types.h>
#include <sys/socket.h>

# define BUFFERSIZE 1024

Client::Client() : _clientFd(-1),
	_bytesRead(-1),
	_buffer(""),
	_bodyStatus(-1),
	_config(NULL),
	_isDisconnected(false),
	_ParsingIsDone(false)
{}

Client::Client(int fd, ServerConfig* config) :
	_clientFd(fd),
	_bytesRead(-1),
	_buffer(""), \
	_bodyStatus(-1),
	_config(config),
	_isDisconnected(false),
	_ParsingIsDone(false)
{}

Client::Client(const Client& other) : _clientFd(other._clientFd),
	_bytesRead(other._bytesRead),
	_buffer(other._buffer),
	_bodyStatus(other._bodyStatus),
	_config(other._config),
	_isDisconnected(other._isDisconnected),
	_ParsingIsDone(other._ParsingIsDone) {}

bool Client::recvSucceeded()
{
	char buffer[1024];
	this->_bytesRead = recv(_clientFd, buffer, sizeof(_buffer), MSG_DONTWAIT);
	if (_bytesRead == 0)
	{
		_isDisconnected = true;
		return false; // 
	}
	if (this->_bytesRead == -1)
	{
		return (false);
	}
	_buffer += std::string(buffer);
	_ParsingIsDone = true; // change this later
	return (true);
}

void	Client::requestCompete()
{
	size_t pos = this->_buffer.find("\r\n\r\n");
	if (pos == std::string::npos) // npos indicates the not found state
	{
		return;
	}

	// if (_ParsingStatus == REQUESTLINE)
	// 	parseRequestLine(); // set to header
	
	// if (_ParsingStatus = HEADER)
	// 	parseHEadeers(); // set to body

	// if (_ParsingStatus == BODY)
	// 	defineBody(); // set bool _isdone
}

int		Client::getParsingIsDone() const
{
	return (this->_ParsingIsDone);
}

Client::~Client() {}
