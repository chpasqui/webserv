#include <unistd.h>
#include <iostream>

void	safeClose(int fd)
{
	if (fd == -1)
		return ;
	std::cout << "entered the safeclose for fd" << fd << std::endl;
	close(fd);
	fd = -1;
}
