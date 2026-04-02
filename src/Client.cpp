#include "Client.hpp"
#include "ServerConfig.hpp"
#include <sys/types.h>
#include <sys/socket.h>

# define BUFFERSIZE 1024

Client::Client() :
	_clientFd(-1),
	_bytesRead(-1),
	_buffer(""),
	_bodyStatus(-1),
	_config(NULL),
	_clientStatus(-1) {}

Client::Client(int fd, ServerConfig* config) :
	_clientFd(fd),
	_bytesRead(-1),
	_buffer(""), \
	_bodyStatus(-1),
	_config(config),
	_clientStatus(-1) {}

Client::Client(const Client& other) :
	_clientFd(other._clientFd),
	_bytesRead(other._bytesRead),
	_buffer(other._buffer),
	_bodyStatus(other._bodyStatus),
	_config(other._config),
	_clientStatus(-1) {}

// bool Client::recvSucceeded()
// {
// 	char buffer[1024];
// 	this->_bytesRead = recv(_clientFd, buffer, sizeof(_buffer), MSG_DONTWAIT);
// 	if (_bytesRead == 0)
// 	{
// 		//potentially add the safe close
		
// 		return false; // 
// 	}
// 	if (this->_bytesRead == -1)
// 	{
// 		//safe close 
// 		// epoll ctl
// 		return (false);
// 	}
// 	_buffer += std::string(buffer);
// 	_ParsingIsDone = true; // change this later
// 	return (true);
// }

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

Client::~Client() {}
