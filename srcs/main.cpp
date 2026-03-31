#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <netinet/in.h>
#include "../include/color.hpp"
#include "../include/Epoll.hpp"

int main(int ac, char **av)
{
	(void) ac;
	(void) av;

	try {
		Epoll epoll = Epoll::getInstance();
	}
	catch (std::runtime_error &e) {
		std::cerr << C_RED << e.what() << RESET << std::endl;
	}
	

	return (0);
}
