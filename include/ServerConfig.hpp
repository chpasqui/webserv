#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <string>
#include <vector>

class Location;

class ServerConfig
{
	private:
		bool						_autoindex;
		int							_port;
		std::string					_route;
		std::string					_allowedMethods;
		std::vector<Location *>		_locations;
		std::vector<std::string>	_index;
	public:
		ServerConfig();
		~ServerConfig();
		bool getAutoIndex(void) const;
		int getPort(void); //WARN: "const"" here ?
		std::string getAllowedMethods(void) const;
		std::string getRoute(void) const;
		void setPort(const int port);
		void setRoute(const std::string& route);
};


#endif
