#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <string>
#include <vector>

class Http;
class ServerConfig;
class Location;

enum context {
	NONE,
	HTTP,
	SERVER,
	LOCATION,
};

class ParseConfig {
	public:
		ParseConfig();
		ParseConfig(std::string filename);
		~ParseConfig();
		
		int				parsingConfigFile(std::vector<ServerConfig*> &serversConfigs);
	private:
		int				openFile(void);
		std::string		getNextToken(void);
		//----ProcessToken----//
		int				processToken(std::vector<ServerConfig*> &serversConfigs);
		int				processTokenHttp(void);
		int				processServerToken(std::vector<ServerConfig *> &servers);
		int				processLocationToken(void);
		int				processRootToken(void);
		int				processListenToken(void);
		int				processErrorPageToken(void);
		
		std::string		_filename;
		std::string		_configFile;
		int				_context;
		Http			*_http;
		ServerConfig	*_currentServer;
		Location		*_location;
};

#endif
