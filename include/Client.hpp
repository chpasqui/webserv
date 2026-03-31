#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <map>

class Client
{
	public:
		Client();
		Client(const Client& other);
		~Client();
	private:
		int						_clientFd;
		size_t					_bytesRead;
		std::string				_buffer;
		int						_bodyStatus;
		
};

#endif
