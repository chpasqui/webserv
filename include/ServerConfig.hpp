#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <string>
#include <vector>

class Http;
class Location;

class ServerConfig
{
	private:
		ServerConfig();

		int							_port;
		std::string					_serverName; //WARN: Only for virtual server ?
		std::vector<Location *>		_locations;

		// int							_headerTimeout;
		// int							_bodyTimeout;
		// int							_maxBodysize;
		bool						_autoindex;
		std::string					_route;
		std::string					_allowedMethods;
		std::vector<std::string>	_indexs;
	public:
		ServerConfig(const Http &http);
		~ServerConfig();
		//----Setter----//
		void						setPort(const int port);
		void						setServerName(const std::string serverName);
		void						addLocation(Location *location);
		void						setAutoIndex(bool autoindex);
		void						setRoute(const std::string& route);
		void						setAllowedMethods(std::string allowedMethods);
		void						addIndex(const std::string index);
		//----Getter----//
		int							getPort(void) const;
		std::string					getServerName(void) const;
		std::vector<Location *>		getLocations(void) const;
		bool						getAutoIndex(void) const;
		std::string					getRoute(void) const;
		std::string					getAllowedMethods(void) const;
		std::vector<std::string>	getIndexs(void) const;
};

#endif
