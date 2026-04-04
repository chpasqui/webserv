#include "../include/Server.hpp"
#include "../include/ServerConfig.hpp"

#include <vector>
#include <iostream>

int main() {
	std::vector<ServerConfig> configs;
	ServerConfig config1;
	config1.setPort(8080);
	config1.setRoute("/var/www");

	ServerConfig config2;
	config2.setPort(9090);
	config2.setRoute("/api");

	configs.push_back(config1);
	configs.push_back(config2);
	
	try
	{
		Server server(configs); //INFO: Singleton class for Server ?
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
	 * 1. create a new socket for the listening fd
	 * 2. bind the listening fd to the corresponding port in the configuration file. The port number is used by the kernel to match an incoming packet to a certin process's socket descriptotr
	 * 3. set it to listen to incoming connecitons on a specific port. Backllog says how many connections are allower to queue up
	 */
	//	Server server(ServerConfigs);

	/**
	 * @brief server.run is the main multiplexer loop though which everything passes. 
	 * 1. a client (someone) will try to connecct on your machine or port andd that we are 
	 * listen()ing on. The connection will be queued up waiting to be accept()ed. 
	 * 2. when calling accept() we tell it to get the corresponding pending connection which 
	 * will return a brand new client descritor to use for that single connection. The original
	 * listenfd will continue to listen for new connections. The new socket is ready for recv() and send()
	 * 3. epol will monitor lostenfd to see of any I/O is possible on any of them. The epoll_wait just tells us 
	 * which fd is ready
	 */
	//	server.run()
	//	return 0
//}
