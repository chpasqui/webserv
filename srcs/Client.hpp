#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <map>

class Client
{
	private:
		int						_clientFd;
		ssize_t					_bytesRead;
		std::string				_buffer;
		int						_bodyStatus;
	public:
		Client();
		~Client();
};

#endif
