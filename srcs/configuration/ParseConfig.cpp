#include "../../include/ParseConfig.hpp"
#include <algorithm>
#include <errno.h>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "../../include/Http.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Location.hpp"
#include "../../include/color.hpp"
#include "../../include/Debug.hpp"

ParseConfig::ParseConfig() :
	_filename(""),
	_configFile(""),
	_line(0),
	_contextLevel(0),
	_contextConfig(0),
	_http(0),
	_currentConfig(0),
	_location(0),
	_serverConfigs(0),
	_dictionary() {
	LOG(C_CSTR << "ParseConfig: Default constructor called" << RESET);
}

ParseConfig::ParseConfig(const std::string &filename, std::vector<ServerConfig *> *serverConfigs) :
	_filename(filename),
	_configFile(""),
	_line(1),
	_contextLevel(NONE),
	_contextConfig(0),
	_http(0),
	_currentConfig(0),
	_location(0),
	_serverConfigs(serverConfigs) {
	_dictionary["}"] = &ParseConfig::processEndBraquetToken;
	_dictionary["http"] = &ParseConfig::processTokenHttp;
	_dictionary["server"] = &ParseConfig::processServerToken;
	_dictionary["location"] = &ParseConfig::processLocationToken;
	_dictionary["client_header_timeout"] = &ParseConfig::processHeaderTimeoutToken;
	_dictionary["client_body_timeout"] = &ParseConfig::processBodyTimeoutToken;
	_dictionary["client_max_body_size"] = &ParseConfig::processMaxBodySizeToken;
	_dictionary["root"] = &ParseConfig::processRootToken;
	_dictionary["listen"] = &ParseConfig::processListenToken;
	_dictionary["autoindex"] = &ParseConfig::processAutoindexToken;
	_dictionary["error_page"] = &ParseConfig::processErrorPageToken;
	_dictionary["server_name"] = &ParseConfig::processServerNameToken;
	_dictionary["allowed_methods"] = &ParseConfig::processAllowedMethodsToken;
	_dictionary["index"] = &ParseConfig::processIndexToken;
	_dictionary["return"] = &ParseConfig::processRedirectionToken;
	_dictionary["cgi_pass"] = &ParseConfig::processCgiPassToken;
	_dictionary["upload_store"] = &ParseConfig::processUploadStoreToken;
	LOG(C_CSTR << "ParseConfig: Normal construtor called" << RESET);
}

ParseConfig::~ParseConfig() {
	LOG(C_DSTR << "ParseConfig: Destructor called" << RESET);
	//TODO: Safely destroy non-null pointers
}

int	ParseConfig::parsingConfigFile(void) {
	std::string														token;
	std::map<std::string, void (ParseConfig::*)(void)>::iterator	it;

	errno = 0;
	if (this->openFile() != 0)
		return (1);

	while (true)
	{
		if (!this->getNextToken(token))
			break;
		it = this->_dictionary.find(token);
		if (it == this->_dictionary.end()) {
			std::cerr << C_RED << token << ": Unknow token on line: " << this->_line << RESET  << std::endl;
			return (1);
		}
		
		try {
			(this->*it->second)();
		}
		catch (std::exception &e) {
			std::cerr << C_RED << e.what() << "on line: " << this->_line << RESET << std::endl;
			delete this->_http; //FIX: Potential leak if the exception is thrown
			//TODO: free serverConfigsVector
			return (1);
		}
	}
	if (this->_serverConfigs == 0) {
		std::cerr << C_RED << "Error: no config file provided" << RESET << std::endl;
		return (1);
	}
	std::cout << C_GREEN << "End of configFile: no error" << RESET << std::endl;
	return (0);
}

int		ParseConfig::openFile(void) {
	std::string				buffer;
	std::string::iterator	it;

	errno = 0;
	if (this->_filename.size() <= FILENAME_EXT_SIZE || this->_filename.compare(this->_filename.size() - FILENAME_EXT_SIZE, FILENAME_EXT_SIZE, FILENAME_EXT)) {
		std::cerr << C_RED << "invalid extension, you're filename need to end with: \"" << FILENAME_EXT << "\"" << RESET << std::endl;
		return (1);
	}

	this->_configFile.open(this->_filename.c_str());
	if (!this->_configFile.is_open()) {
		std::cerr << C_RED << "Unable to open the file " << this->_filename << " :" << strerror(errno) << RESET << std::endl;
		return (errno);
	}
	
	return (0);
}

//TODO:
//- Add a default location if no onne is given. OK
//- Ignore the end ligne with " #". OK
//- Add a line number for errors.
//- Display a better error message.
//- check if the file end with ".conf". OK
//- Fix leaks when exception is thrown.
//- Rename variables with wrong names
//- Read word by word the config file OK
//- Ajuste the size of bodySize in config file (1m = * 1'000'000), the parsin check the numbers of chars.
//- remove the setPath function in location
//- add default index.html if no one is provided

/**
 * @biref get the next token in the config file and remove it from the file string
 * @return the token or an empty string if the end of the file has been reached
 */
int ParseConfig::getNextToken(std::string &token) {
	std::string::iterator it;
	this->_configFile >> token;
	if (this->_configFile.eof() || this->_configFile.fail())
		return (0);
	
	if (*(token.begin()) == '#')
		this->_configFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	LOG(C_PINK << "Token: " << token << RESET);
	return (1);
}

bool	ParseConfig::thereIsDefaultLocation(void) const {
	std::vector<Location*> locations;

	if (!this->_currentConfig)
		throw (std::logic_error("No server config to check for default location"));

	locations = this->_currentConfig->getLocations();
	for (std::vector<Location *>::iterator it = locations.begin(); locations.end() != it; it++) {
		if ((*it)->getPath() == "/")
			return (true);
	}
	return (false);
}

bool	ParseConfig::thereIsIndex(void) const {
	if (!this->_location)
		throw (std::logic_error("No location to check for index"));

	if (this->_location->getIndexes().empty())
		return (false);
	return (true);
}

void	ParseConfig::processEndBraquetToken(void) {
	switch (this->_contextLevel) {
		case HTTP:
			delete (this->_http);
			this->_http = 0;
			break;
		case SERVER:
			if (!this->thereIsDefaultLocation()) {
				LOG(C_PINK << "No default location found, create one" << RESET);
				this->_currentConfig->addLocation(new Location(*(this)->_currentConfig, "/"));
			}
			this->_currentConfig = 0;
			break;
		case LOCATION:
			if (!this->thereIsIndex()) {
				LOG(C_PINK << "No index found for location " << this->_location->getPath() << ", add default index \"index.html\"" << RESET);
				this->_location->addIndex("index.html");
			}
			this->_location = 0;
			break;
		default:
			throw std::runtime_error("Unexcepted token \"}\"";
	}
	this->_contextLevel -= 1;
}

void	ParseConfig::processTokenHttp(void) {
	std::string	braquet;

	if (!getNextToken(braquet))
		throw (std::invalid_argument("fail to get a token"));
	if (braquet != "{" || this->_contextLevel != NONE || this->_http)
		throw (std::runtime_error("Unexcepted token after \"http\""));
	this->_http = new Http();
	this->_contextConfig = this->_http;
	this->_contextLevel = HTTP;
	}

void	ParseConfig::processServerToken(void) {
		std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if (nextToken != "{" || this->_contextLevel != HTTP || this->_currentConfig)
		throw (std::runtime_error("Unexcepted token after \"server\""));
	this->_serverConfigs->push_back(new ServerConfig( *(this)->_http));
	this->_currentConfig = *(this)->_serverConfigs->rbegin();
	this->_contextConfig = this->_currentConfig;
	this->_contextLevel = SERVER;
}

void	ParseConfig::processLocationToken(void) {
	std::string	path;
	std::string	braquet;

	if (!getNextToken(path) || !getNextToken(braquet))
		throw (std::invalid_argument("fail to get a token"));
	if (this->_location)
		throw std::runtime_error("Nested location is not allowed");

	this->_location = new Location(*(this)->_currentConfig, path);
	this->_contextConfig = this->_location;
	
	if (braquet != "{" || this->_contextLevel!= SERVER)
		throw std::runtime_error("Unexcepted token after \"Location\"");

	this->_contextLevel = LOCATION;
	this->_currentConfig->addLocation(this->_location);
}

/**
 * @detail client_header_timeout context: http, server
 */
void	ParseConfig::processHeaderTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return ;
	this->_contextConfig->setHeaderTimeout(timeout);
}

/**
 * @detail client_body_timeout context: http, server, location
 */
void	ParseConfig::processBodyTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;


	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return ;
	this->_contextConfig->setBodyTimeout(timeout);
	return ;
}

/**
 * @detail client_max_body_size context: http, server, location
 */
void	ParseConfig::processMaxBodySizeToken(void) {
	int			maxBodySize;
	std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if ((maxBodySize = ParseConfig::transformTokenToInt(nextToken)) == -1) //FIX: need to mutiplie this later
		return ;
	this->_contextConfig->setMaxBodySize(maxBodySize);
}

/**
 * @detail root context: http, server, location
 */
void	ParseConfig::processRootToken(void) {
	std::string nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	this->_contextConfig->setRoot(nextToken);
	return;
}

/**
 * @detail listen context: server
 */
void	ParseConfig::processListenToken(void) {
		int 		port;
		std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if (this->_contextLevel != SERVER)
	{
		std::cerr << C_RED << "Using token \"listen\" in wrong context" << RESET << std::endl;
		return;
	}
	if ((port = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return;
	this->_contextConfig->setPort(port);
	return;
}

/**
 * @detail autoindex context: http, server, location
 * @detail only response possible on | off
 */
void	ParseConfig::processAutoindexToken(void) {
	std::string value;

	if (!getNextToken(value))
		throw (std::invalid_argument("fail to get a token"));
	if (value == "on")
		this->_contextConfig->setAutoindex(true);
	else if (value == "off")
		this->_contextConfig->setAutoindex(false);
	else
		throw std::runtime_error("value " + value + " unrecognized after autoindex");
	return;
}

/**
 * @detail error_page context: http, server, location
 */
void	ParseConfig::processErrorPageToken(void) {
	int			code;
	std::string	codeToken, pageToken;

	if (!getNextToken(codeToken) || !getNextToken(pageToken))
		throw (std::invalid_argument("fail to get a token"));

	code = ParseConfig::transformTokenToInt(codeToken);
	this->_contextConfig->addErrorPage(code, pageToken);
}

/**
 * @detail server_name context: server
 */
void	ParseConfig::processServerNameToken(void) {
	std::string serverName;

	if (!getNextToken(serverName))
		throw (std::invalid_argument("fail to get a token"));
	this->_contextConfig->setServerName(serverName);
}


void	ParseConfig::processAllowedMethodsToken(void) {
	std::string allowedMethods;

	if (!getNextToken(allowedMethods))
		throw (std::invalid_argument("fail to get a token"));
	this->_contextConfig->setAllowedMethods(allowedMethods);
}

void	ParseConfig::processIndexToken(void) {
	std::string index;

	if (!getNextToken(index))
		throw (std::invalid_argument("fail to get a token"));
	this->_contextConfig->addIndex(index);
}

void	ParseConfig::processRedirectionToken(void) {
	int			code;
	std::string	codeToken;
	std::string	redirectionPath;

	if (!getNextToken(codeToken) || !getNextToken(redirectionPath))
		throw (std::invalid_argument("fail to get a token"));
	code = ParseConfig::transformTokenToInt(codeToken);
	this->_contextConfig->setRedireCode(code);
	this->_contextConfig->setRedirePath(redirectionPath);
}

void	ParseConfig::processCgiPassToken(void) {
	std::string	extension;
	std::string	path;

	if (!getNextToken(extension) || !getNextToken(path))
		throw (std::invalid_argument("fail to get a token"));
	if (*(extension.begin()) != '.')
		throw (std::invalid_argument("an extension need to start with a point"));
	this->_contextConfig->addCgiExtension(extension, path);
}

void	ParseConfig::processUploadStoreToken(void) {
	std::string uploadStorePath;

	if (!getNextToken(uploadStorePath))
		throw (std::invalid_argument("fail to get a token"));
	this->_contextConfig->setUploadStore(uploadStorePath);
}

/**
 * @brief transform a token to a usable int
 * @param the token you want to convert
 * @return the converted int or -1 if an error occurs 
 */
int	ParseConfig::transformTokenToInt(std::string token) { //OPTIM: Use std::stringstream to get ints
	int					nb;
	std::stringstream	ss(token);
	
	ss >> nb;
	if (ss.fail() || !ss.eof() || nb < 0)
		throw std::runtime_error("invalid number in line x");
	//LOG(C_PURPLE << "DEBUG: number is: " << nb << RESET);
	return (nb);
}
