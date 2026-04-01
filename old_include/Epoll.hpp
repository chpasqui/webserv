#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <sys/epoll.h>

class Epoll
{
	public:
		static Epoll &getInstance(void);
		~Epoll();

		int		getEpollFd(void) const;
	private:
		Epoll();
		int	_epollFd;
};

#endif
