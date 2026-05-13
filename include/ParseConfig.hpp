#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <fstream>
#include <map>
#include <string>
#include <sstream>
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
		ParseConfig(std::vector<ServerConfig *> *serverConfigs);
		~ParseConfig();
		
		int													parsingConfigFile(const std::string &filename);
	private:
		int													openFile(const std::string &filename);
		int													getNextToken(std::string &token);
		int													getNextToken(int &token);
		bool												thereIsDefaultLocation(void) const;
		bool												thereIsIndex(void) const;
		//----ProcessToken----//
		void												processEndBraquetToken(void);
		void												processTokenHttp(void);
		void												processServerToken(void);
		void												processLocationToken(void);
		void												processHeaderTimeoutToken(void);
		void												processBodyTimeoutToken(void);
		void												processMaxBodySizeToken(void);
		void												processRootToken(void);
		void												processListenToken(void);
		void 												processAutoindexToken(void);
		void												processServerNameToken(void);
		void												processAllowedMethodsToken(void);
		void												processIndexToken(void);
		void												processErrorPageToken(void);
		void												processRedirectionToken(void);
		void												processCgiPassToken(void);
		void												processUploadStoreToken(void);
		//----Atributes----//
		std::ifstream										_configFile;
		int													_contextLevel;
		AConfig												*_contextConfig;
		Http												*_http;
		ServerConfig										*_currentConfig;
		Location											*_location;
		std::vector<ServerConfig*>							*_serverConfigs;
		std::map<std::string, void (ParseConfig::*)(void)>	_dictionary;
};

#endif
