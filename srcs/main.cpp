#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/server.hpp"

int main(int ac, char **av)
{
	(void) ac;
	(void) av;
	Server testServer;

	try {
		testServer.createListeningSocket();
		testServer.bindAndListen();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	//init epoll
	handleClientsConnections(testServer.getListenFd());
	close(testServer.getListenFd());
	return (0);
}
