#include "../include/color.hpp"
#include "../include/ParseConfig.hpp"
#include "../include/Server.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Cgi.hpp"

#include <vector>
#include <iostream>

int main(int ac, char **av) {
	if (ac != 2)
		return 1;
	
	std::vector<ServerConfig*> configs;
	ParseConfig pc(av[1], &configs);
	pc.parsingConfigFile();


	ServerConfig *serverconfig = *(configs.begin());

	std::cout << "Route is " << serverconfig->getRoute() << " Port is " << serverconfig->getPort() << std::endl;
	
	Server server;

	try
	{
		std::cout << "entered the try tactch block" << std::endl;
		server.setupListeningSockets(configs);
		server.run(); // server instance out of scope if declared outside, I need it here but run does not 
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
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
