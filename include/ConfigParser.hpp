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
		class												EndOfConfigFile;
		int													openFile(const std::string &filename);
		void												getNextToken(std::string &token);
		void												getNextLine(void);
		bool												thereIsDefaultLocation(void) const;
		bool												thereIsIndex(void) const;
		static int											strToInt(const std::string &str);
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

class ConfigParser::EndOfConfigFile : public std::exception {
	public:
		virtual const char *what() const throw();
};

#endif
