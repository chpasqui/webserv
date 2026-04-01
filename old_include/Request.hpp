#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>

class Request
{
	public:
		Request();
		Request(std::string requestMessage, int fd);
		~Request();

		//checkEnd()
		void	requestType(void);
	private:
		std::string	_requestMessage;
		std::string	_response;
		int			_socketId;

		void	sendReponse(void);
};

#endif
