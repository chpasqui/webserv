#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <map>
#include <sys/types.h>

#include "ServerConfig.hpp"

enum ParsingStatus
{
	REQUESTLINE,
	HEADER,
	BODY, 
	DONE
};

class Client
{
	public:
		Client();
		Client(int fd, ServerConfig* config);
		Client(const Client& other);
		~Client();
		bool	recvSucceeded();
		void	requestCompete();
		int		getParsingIsDone() const;
	private:
		int						_clientFd;
		ssize_t					_bytesRead; // ssize so that it can be compared to -1 
		std::string				_buffer;
		int						_bodyStatus;
		ServerConfig*			_config;
		bool					_isDisconnected;
		bool					_ParsingIsDone;
		// bool					_ParsingStatus;
};

#endif
