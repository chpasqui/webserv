#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/Request.hpp"
#include "../include/Response.hpp"
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
	public:
		Client(int fd, const ServerConfig* config, Request *request);
		Client(const Client& other);
		~Client();

		void				parseRequest();
		bool				bodyRead(std::string& buffer);

		int					getClientFd() const;
		int					getParsingStatus() const;
		int					getCgiPipeFd(void) const;
		void				setCgiPipeFd(int fd);
		Request*			getRequest(void) const;
		const ServerConfig* getServerConfig(void) const;
		time_t				getTimestamp(void) const;
		void				buildResponse(char **envp);
		std::string 		getResponse() const;

		


	private:
		Client(); // client constructor is in private so that we never create a client out of nowhere
		int					_clientFd;
		int					_cgiPipeFd;
		int					_writeCgiPipeEnd;
		int					_readCgiPipeEnd;

		ssize_t				_bytesRead;
		std::string			_buffer;
		id_t				_parsingStatus;

		time_t				_timestamp;

		const ServerConfig*	_config;
		Request*			_request;
		Response*			_response;
};

#endif
