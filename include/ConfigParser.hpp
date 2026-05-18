#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <fstream>
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

class ConfigParser {
	public:
		ConfigParser();
		ConfigParser(std::vector<ServerConfig *> *serverConfigs);
		~ConfigParser();
		
		int													parsingConfigFile(const std::string &filename);
	private:
		int													openFile(const std::string &filename);
		int													getNextToken(std::string &token);
		int													getNextLine(void);
		bool												thereIsDefaultLocation(void) const;
		bool												thereIsIndex(void) const;
		//----ProcessToken----//
		void												processEndBraquetToken(void);
		void												processTokenHttp(void);
		void												processServerToken(void);
		void												processLocationToken(void);
		void												processClientTimeoutToken(void);
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
		//----Atributes----//
		std::ifstream										_configFile;
		std::string											_currentLine;
		int													_contextLevel;
		int													_lineNumber;
		AConfig												*_currentContext;
		Http												*_http;
		ServerConfig										*_currentServer;
		Location											*_currentLocation;
		std::vector<ServerConfig*>							*_serverConfigs;
		std::map<std::string, void (ConfigParser::*)(void)>	_dictionary;
};

#endif
