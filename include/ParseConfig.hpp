#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <map>
#include <string>
#include <vector>

#define FILENAME_EXT ".conf"
#define FILENAME_EXT_SIZE 5

class Http;
class ServerConfig;
class Location;
class AConfig;

enum context {
	NONE,
	HTTP,
	SERVER,
	LOCATION,
};

class ParseConfig {
	public:
		ParseConfig();
		ParseConfig(const std::string &filename, std::vector<ServerConfig *> *serverConfigs);
		~ParseConfig();
		
		int				parsingConfigFile(void);
	private:
		int				openFile(void);
		std::string		getNextToken(void);
		bool			thereIsDefaultLocation(void) const;
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
		void				processServerNameToken(void);
		void				processAllowedMethodsToken(void);
		void				processIndexToken(void);
		void				processErrorPageToken(void);
		void				processRedirectionToken(void);
		void				processCgiPassToken(void);
		void				procesaUploadStoreToken(void);
		//----Static functions----//
		static int		transformTokenToInt(std::string token);
		//----Atributes----//
		std::string											_filename;
		std::string											_configFile;
		int													_line;
		int													_contextLevel;
		AConfig												*_contextConfig;
		Http												*_http;
		ServerConfig										*_currentConfig;
		Location											*_location;
		std::vector<ServerConfig*>							*_serverConfigs;
		std::map<std::string, void (ParseConfig::*)(void)>	_dictionary;
};

#endif
