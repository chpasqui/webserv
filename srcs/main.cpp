#include <algorithm>
#include "../include/color.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Server.hpp"
#include "../include/Location.hpp"
#include "../include/ServerConfig.hpp"

#include <vector>
#include <iostream>

static int	checkInputArgs(int ac, char **av, std::string &filename);
static int	checkResponse(const std::string &response);

/**
 * @brief Server(Serverconfigs) will loop over the vector of serverConfigs and will
 * 1. create a new socket for the listening fd
 * 2. bind the listening fd to the corresponding port in the configuration file. The port number is used by the kernel to match an incoming packet to a certin process's socket descriptotr
 * 3. set it to listen to incoming connecitons on a specific port. Backllog says how many connections are allower to queue up
 */
int main(int ac, char **av) {
	std::string configFileName;
	std::vector<ServerConfig*> configs;
	//std::srand(static_cast<unsigned int>(std::time(NULL))); //voir si laisser la

	{
		if (checkInputArgs(ac, av, configFileName) == 1)
			return (1);

		ConfigParser pc(&configs);
		if (pc.parsingConfigFile(configFileName) == 1)
			return (1);
	}

	Server server;

	try {
		server.setupListeningSockets(configs);
		std::cout << C_BLUE << "The server listens on the following ports: \n";
		for (std::vector<ServerConfig*>::iterator it = configs.begin(); it != configs.end(); it++)
			std::cout << (*it)->getPort() << "\n";
		server.run(); // server instance out of s cope if declared outside, I need it here but run does not 
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return (1);
	}
	
	return (0);
}

/**
 * @brief Checks the number of parameters passed to the program.
 * If no parameters are provided, it asks the user if they want to use the default configuration file.
 * based on the user's input
 * @return 0 if successful and 1 if unsuccessful
 *
 * @warning The default configuration file must exist; otherwise, an error will be returned
 */
static int	checkInputArgs(int ac, char **av, std::string &filename) {
	int			status;
	std::string	response;

	if (ac > 2) {
		std::cerr << C_YELLOW << "This program can only use a single configuration file, stopping webserv" << RESET << std::endl;
		return 1;
	}
	else if (ac == 1) {
		int i = 0;
		do {
			std::cout << C_BLUE << "No configuration file was provided. Would you like to try using the default one ?\n[Y/n]" <<  RESET << std::endl;
			std::getline(std::cin, response);
			std::transform(response.begin(), response.end(), response.begin(), tolower);
			status = checkResponse(response);
			i++;
		}
		while (!status && i < 5);
		if (status == 1)
			filename = "config/complete.conf";
		else {
			std::cerr << C_YELLOW << "no configuration file provided. stopping webserv" << RESET << std::endl;
			return (1);
		}
	}
	else
		filename = av[1];
	return (0);
}

static int	checkResponse(const std::string &response) {
	if (response == "y" || response == "yes" || response.empty())
		return (1);
	if (response == "n" || response == "no")
		return (2);
	return (0);
}

