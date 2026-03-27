#include "../include/Request.hpp"
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


Request::Request()
{
	return ;
}

Request::Request(std::string requestMessage, int fd) : _requestMessage(requestMessage), _socketId(fd)
{
	return ;
}

Request::~Request()
{
	return ;
}

void	Request::requestType(void)
{
	if (this->_requestMessage.find("GET"))
	{
		this->sendReponse();
	}
}

void	Request::sendReponse(void)
{
	int		file ;
	char	buffer[4096];
	ssize_t	bytesRead;

	file = open("www/homepage.html" ,O_RDONLY);
	if (file == -1)
		throw (std::runtime_error("Open file error"));
	bytesRead = read(file, buffer, 4095);
	if (bytesRead == -1 || bytesRead == 4095)
		throw (std::runtime_error("Incorrect read or too large buffer"));
	buffer[bytesRead] = '\0';

	if (send(this->_socketId, buffer, bytesRead, MSG_DONTWAIT) == -1)
		throw (std::runtime_error("send error"));
	printf("%s\n", buffer);

}
