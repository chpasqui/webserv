#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <string>

class ServerConfig
{
	private:
		int			_port;
		std::string	_route;

	public:
		ServerConfig();
		~ServerConfig();
		int getPort(void);
		void setPort(const int port);
		void setRoute(const std::string& route);
};


#endif