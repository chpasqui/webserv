#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ctime>
#include <string>
#include <sys/types.h>

class ServerConfig;

enum ParsingStatus
{
	REQUESTLINE,
	HEADER,
	BODY, 
	DONE
};

class Client
{
	private:
		int						_clientFd;
		ssize_t					_bytesRead; // ssize so that it can be compared to -1 
		std::string				_buffer;
		int						_bodyStatus;
		ServerConfig*			_config;
		bool					_isDisconnected;
		bool					_ParsingIsDone;
		// bool					_ParsingStatus;
		std::tm					_timestamp;
	public:
		Client();
		Client(int fd, ServerConfig* config);
		Client(const Client& other);
		~Client();
		bool	recvSucceeded();
		void	requestCompete();
		int		getParsingIsDone() const;
};

#endif
