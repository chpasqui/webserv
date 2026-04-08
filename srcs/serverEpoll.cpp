#include "../include/Server.hpp"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <string>

#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>

/**
 * @brief Registers fdToModify with the epoll instance to monitor it for the
 * events specified by flag (e.g. EPOLLIN, EPOLLOUT).
 */
void	Server::addToEpoll(int epollFd, int fdToModify, uint32_t flag)
{
	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fdToModify, &event) == -1)
	{
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

/**
 * @brief Modifies fdToModify's existing flag with the new flag (e.g. EPOLIN or EPOLLOUT)
 */
void	Server::modifyEpoll(int epollFd, int fdToModify, uint32_t flag)
{

	struct epoll_event	event;
	event.events = flag;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fdToModify, &event) == -1)
	{
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}

/**
 * @brief Removes the fd from the epoll instance to stop monitoring it.
 */
void	Server::delFromEpoll(int epollFd, int fdToModify)
{
	struct epoll_event	event;
	event.events = 0;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fdToModify, &event) == -1)
	{
		//WARN: if (errno == ENOENT) //don't close an inexistant file (Double close);
		//			return; 
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw (std::runtime_error(errorMsg));
	}
}
