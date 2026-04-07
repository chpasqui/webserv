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
	COMPLETE,
	CLOSEFD
};

class Client
{
	private:
		int						_clientFd;
		ssize_t					_bytesRead; // ssize so that it can be compared to -1 
		std::string				_buffer;
		ServerConfig*			_config;
		id_t					_clientStatus;
	public:
		Client();
		Client(int fd, ServerConfig* config);
		Client(const Client& other);
		~Client();
		int		getClientFd() const;
		int		getClientStatus() const;
		void	handleRequest();
};

#endif
