#include <algorithm>
#include "../include/color.hpp"
#include "../include/ParseConfig.hpp"
#include "../include/Server.hpp"
#include "../include/Location.hpp"
#include "../include/ServerConfig.hpp"

#include <vector>
#include <iostream>

int	checkInputArgs(int ac, char **av, std::string &filename);
int	checkResponse(const std::string &response);

int main(int ac, char **av, char **env) {
	std::string configFileName;
	std::vector<ServerConfig*> configs;
	std::srand(static_cast<unsigned int>(std::time(NULL))); //voir si laisser la

	if (checkInputArgs(ac, av, configFileName) == 1)
		return (1);

	ParseConfig pc(configFileName, &configs);
	if (pc.parsingConfigFile() == 1)
		return (1);

	ServerConfig *serverconfig = *(configs.begin());
	std::cout << "Route is " << serverconfig->getRoute() << " Port is " << serverconfig->getPort() << std::endl;
	std::vector<Location *> locations = serverconfig->getLocations();

	Server server(env);

	try {
		/**
		 * @brief Server(Serverconfigs) will loop over the vector of serverConfigs and will
		 * 1. create a new socket for the listening fd
		 * 2. bind the listening fd to the corresponding port in the configuration file. The port number is used by the kernel to match an incoming packet to a certin process's socket descriptotr
		 * 3. set it to listen to incoming connecitons on a specific port. Backllog says how many connections are allower to queue up
		 */
		server.setupListeningSockets(configs);
		/**
		 * @brief server.run is the main multiplexer loop though which everything passes. 
		 * 1. a client (someone) will try to connect on your machine or port and that we are 
		 * listen()ing on. The connection will be queued up waiting to be accept()ed. 
		 * 2. when calling accept() we tell it to get the corresponding pending connection which 
		 * will return a brand new client descritor to use for that single connection. The original
		 * listenfd will continue to listen for new connections. The new socket is ready for recv() and send()
		 * 3. epol will monitor listenfd to see of any I/O is possible on any of them. The epoll_wait just tells us 
		 * which fd is ready
		 * 4. When there is an EPOLIN event, we will look for the delimiters and status and parse the requestline,
		 * header, and body.
		 * 5. Once the request is parsed, it will 
		 */
		server.run(); // server instance out of scope if declared outside, I need it here but run does not 
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	
	for (std::vector<ServerConfig*>::iterator it = configs.begin(); it != configs.end(); it++)
		delete (*it);
	return (0);
}

// map <int, *serverconfig> _listeningToConfig

// int main()
// {
	// Config Parse -> vector<ServerConfigs>
	// severconfig[{port 8080, root: /root/www/, (GET,ADD,etc) }, {port: 9090, root: /var/www }] // vector owns the congig and is never changed

	/**
	 * @brief Server(Serverconfigs) will loop over the vector of serverConfigs and will
	 * 1. create a new socket for the listeniing fd
	 * 2. bind the listening fd to the corresponding port in the configuration file. The port number is used by the kernel to match an incoming packet to a certin process's socket descriptotr
	 * 3. set it to listen to incoming connecitons on a specific port. Backllog says how many connections are allower to queue up
	 */
	//	Server server(ServerConfigs);


	//	server.run()
	//	return 0
//}
//
int	checkInputArgs(int ac, char **av, std::string &filename) {
	int			status;
	std::string	response;

	if (ac > 2) {
		std::cerr << C_RED << "Please use only one configuration file" << RESET << std::endl;
		return 1;
	}
	else if (ac == 1) {
		do {
			std::cout << C_BLUE << "No configuration file was provided. Would you like to try using the default one ?\n[Y/n]" <<  RESET << std::endl;
			std::getline(std::cin, response);
			std::transform(response.begin(), response.end(), response.begin(), tolower);
			status = checkResponse(response);
		}
		while (!status);
		if (status == 1)
			filename = "config/default.conf";
		else {
			std::cout << C_RED << "Please provide a configuration file" << RESET << std::endl;
			return (1);
		}
	}
	else
		filename = av[1];
	return (0);
}

int	checkResponse(const std::string &response) {
	if (response == "y" || response == "yes" || response.empty())
		return (1);
	if (response == "n" || response == "no")
		return (2);
	return (0);
}
