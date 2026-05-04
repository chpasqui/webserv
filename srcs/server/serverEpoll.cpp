#include "../../include/Server.hpp"

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
 * @brief Creates the epoll instance used to monitor all file descriptors (listening
 * sockets and client connections) throughout the server's lifetime. Sets the
 * Server private attribute _epollFd to the returned file descriptor
 *
 * @throws std::runtime_error if epoll_create() fails
 */
void Server::createEpollInstance(void)
{
	std::string errorMsg;

	this->epollFd = epoll_create(1);
	if (this->epollFd == -1)
	{
		errorMsg = std::string("Epoll_create() error: ") + strerror(errno);
		throw(std::runtime_error(errorMsg));
	}
}

/**
 * @brief Registers fdToModify with the epoll instance to monitor it for the
 * events specified by flag (e.g. EPOLLIN, EPOLLOUT).
 */
void Server::addToEpoll(int epollFd, int newFd, uint32_t flag)
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = newFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newFd, &event) == -1)
	{
		close(newFd);
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw(std::runtime_error(errorMsg));
	}
}

/**
 * @brief Modifies fdToModify's existing flag with the new flag (e.g. EPOLIN or EPOLLOUT)
 */
bool Server::modifyEpoll(int epollFd, int fdToModify, uint32_t flag)
{

	struct epoll_event event;
	event.events = flag;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fdToModify, &event) == -1)
	{
		close(fdToModify);
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		return false;
	}
	return true;
}

/**
 * @brief Removes the fd from the epoll instance to stop monitoring it.
 */
void Server::delFromEpoll(int epollFd, int fdToModify)
{
	struct epoll_event event;
	event.events = 0;
	event.data.fd = fdToModify;
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fdToModify, &event) == -1)
	{
		// WARN: if (errno == ENOENT) //don't close an inexistant file (Double close);
		//			return;
		close(fdToModify);
		close(epollFd);
		std::string errorMsg = std::string("Epoll_ctl() error: ") + strerror(errno);
		throw(std::runtime_error(errorMsg));
	}
	// if (_clients.count(fdToModify))
	// {
	// 	delete clients[fdToModify];
	// 	_clients.erase(fdToModify);
	// }
}
