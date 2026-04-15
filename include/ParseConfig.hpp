#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <string>
#include <vector>

class ServerConfig;

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
		
		int		parsingConfigFile(std::vector<ServerConfig*> &serversConfigs);
	private:
		int			openFile(void);
		std::string	getNextToken(void);
		int			processToken(ServerConfig &ServerConfig);
		
		std::string	_filename;
		std::string	_configFile;
		int			_context;
};

#endif
