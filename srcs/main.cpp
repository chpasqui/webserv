#include "../include/server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>


int main(int ac, char **av)
{
	(void) ac;
	(void) av;

	int	listenFd = createListeningSocket();
	if (listenFd == -1)
		return (1);
	if (bindListeningSocket(&listenFd) == -1)
		return (1);
	if (listen(listenFd, BACKLOG) == -1) // 1156 listen system call marks as passive socket
	{
		close(listenFd); //safe close
		perror("listen error");
		return (1);
	}

	handleClientsConnetions(listenFd);
	close(listenFd);
	return (0);
}
