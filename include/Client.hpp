#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <map>
#include <sys/types.h>

#include "ServerConfig.hpp"

enum ParsingStatus
{
	PARSING_REQUEST_LINE,
	PARSING_HEADERS,
	PARSING_BODY, 
	COMPLETE
};

class Client
{
	private:
		int						_clientFd;
		ssize_t					_bytesRead; // ssize so that it can be compared to -1 
		std::string				_buffer;
		int						_bodyStatus;
		ServerConfig*			_config;
		id_t					_clientStatus;
		// bool					_isDisconnected;
		// bool					_ParsingIsDone;
		// bool					_ParsingStatus;
	public:
		Client();
		Client(int fd, ServerConfig* config);
		Client(const Client& other);
		~Client();
		// bool	recvSucceeded();
		// void	requestCompete();
		int		getClientStatus() const;
};

#endif
