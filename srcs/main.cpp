#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <netinet/in.h>
#include "../include/color.hpp"
#include "../include/Epoll.hpp"
#include <stdio.h>
#include "../include/Server.hpp"
#include "../include/Client.hpp"


int main(int ac, char **av)
{
	(void) ac;
	(void) av;
	Server testServer;

	try {
		testServer.createListeningSocket();
		testServer.bindAndListen();
		Epoll epoll = Epoll::getInstance();
	}
	catch (std::runtime_error &e) {
		std::cerr << C_RED << e.what() << RESET << std::endl;
		return (1);
	}

	handleClientsConnections(testServer.getListenFd());
	close(testServer.getListenFd());
	return (0);
}
