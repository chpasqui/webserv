#include "../include/Epoll.hpp"
#include <stdexcept>
#include <cstring>

Epoll &Epoll::getInstance(void) {
	static Epoll instance;
	return (instance);
}

Epoll::~Epoll() {
	return ;
}

Epoll::Epoll() {
	std::string	errorMsg;

	errno = 0;
	this->_epollFd = epoll_create(1);
	if (this->_epollFd == -1)
	{
		errorMsg = std::string("Epoll_create error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
	return ;
}

int	Epoll::getEpollFd(void) const {
	return (this->_epollFd);
}
