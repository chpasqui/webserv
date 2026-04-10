#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/Request.hpp"
#include <ctime>
#include <string>
#include <sys/types.h>

class ServerConfig;

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
		Client();

		int						_clientFd;
		ssize_t					_bytesRead;
		std::string				_buffer;
		ServerConfig*			_config;
		id_t					_clientStatus;
		time_t					_timestamp;
		Request*				_request;
		
	public:
		Client(int fd, ServerConfig* config, Request *request);
		Client(const Client& other);
		~Client();

		int		getClientFd() const;
		int		getClientStatus() const;
		void	handleRequest();
		time_t	getTimestamp(void) const;
		bool	bodyRead(std::string& buffer);
};

#endif
