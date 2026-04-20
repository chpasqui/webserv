#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <map>
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
		ParseConfig(std::string filename, std::vector<ServerConfig *> *serverConfigs);
		~ParseConfig();
		
		int				parsingConfigFile(void);
	private:
		int				openFile(void);
		std::string		getNextToken(void);
		//----ProcessToken----//
		void				processEndBraquetToken(void);
		void				processTokenHttp(void);
		void				processServerToken(void);
		void				processLocationToken(void);
		void				processHeaderTimeoutToken(void);
		void				processBodyTimeoutToken(void);
		void				processMaxBodySizeToken(void);
		void				processRootToken(void);
		void				processListenToken(void);
		void 				processAutoindexToken(void);
		void				processErrorPageToken(void);
		//----Static functions----//
		static int		transformTokenToInt(std::string token);
		//----Atributes----//
		std::string											_filename;
		std::string											_configFile;
		int													_context;
		Http												*_http;
		ServerConfig										*_currentConfig;
		Location											*_location;
		std::vector<ServerConfig*>							*_serverConfigs;
		std::map<std::string, void (ParseConfig::*)(void)>	_dictionary;
};

#endif
