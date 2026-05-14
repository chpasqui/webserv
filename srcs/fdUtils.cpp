#include <unistd.h>
#include <iostream>

void	safeClose(int &fd)
{
	if (fd == -1)
		return ;
	close(fd);
	fd = -1;
}
