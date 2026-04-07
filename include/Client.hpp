#ifndef CLIENT_HPP
# define CLIENT_HPP

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
		ssize_t					_bytesRead; // ssize so that it can be compared to -1 
		std::string				_buffer;
		ServerConfig*			_config;
		id_t					_clientStatus;
		time_t					_timestamp;
	public:
		Client(int fd, ServerConfig* config);
		Client(const Client& other);
		~Client();

		int		getClientFd() const;
		int		getClientStatus() const;
		void	handleRequest();
		time_t	getTimestamp(void) const;
};

#endif
