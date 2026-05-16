#include "../../include/ConfigParser.hpp"
#include <errno.h>
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

ConfigParser::ConfigParser() :
	_configFile(""),
	_contextLevel(0),
	_currentContext(0),
	_http(0),
	_currentServer(0),
	_currentLocation(0),
	_serverConfigs(0),
	_dictionary() {
	LOG_DEBUG(C_CSTR << "ConfigParser: Default constructor called" << RESET);
}

ConfigParser::ConfigParser(std::vector<ServerConfig *> *serverConfigs) :
	_configFile(""),
	_contextLevel(NONE),
	_currentContext(0),
	_http(0),
	_currentServer(0),
	_currentLocation(0),
	_serverConfigs(serverConfigs) {
	_dictionary["}"] = &ConfigParser::processEndBraquetToken;
	_dictionary["http"] = &ConfigParser::processTokenHttp;
	_dictionary["server"] = &ConfigParser::processServerToken;
	_dictionary["location"] = &ConfigParser::processLocationToken;
	_dictionary["client_timeout"] = &ConfigParser::processClientTimeoutToken;
	_dictionary["client_max_body_size"] = &ConfigParser::processMaxBodySizeToken;
	_dictionary["root"] = &ConfigParser::processRootToken;
	_dictionary["listen"] = &ConfigParser::processListenToken;
	_dictionary["autoindex"] = &ConfigParser::processAutoindexToken;
	_dictionary["error_page"] = &ConfigParser::processErrorPageToken;
	_dictionary["server_name"] = &ConfigParser::processServerNameToken;
	_dictionary["allowed_methods"] = &ConfigParser::processAllowedMethodsToken;
	_dictionary["index"] = &ConfigParser::processIndexToken;
	_dictionary["return"] = &ConfigParser::processRedirectionToken;
	_dictionary["cgi_pass"] = &ConfigParser::processCgiPassToken;
	LOG_DEBUG(C_CSTR << "ConfigParser: Normal construtor called" << RESET);
}

ConfigParser::~ConfigParser() {
	if (_http)
		delete (_http);
	_configFile.close();
	LOG_DEBUG(C_DSTR << "ConfigParser: Destructor called" << RESET);
}

int	ConfigParser::parsingConfigFile(const std::string &filename) {
	std::string														token;
	std::map<std::string, void (ConfigParser::*)(void)>::iterator	it;

	errno = 0;
	if (openFile(filename) != 0)
		return (1);

	while (true)
	{
		if (!getNextToken(token))
			break;
		it = _dictionary.find(token);
		if (it == _dictionary.end()) {
			std::cerr << C_RED << token << ": Unknow token on line: " << RESET  << std::endl;
			return (1);
		}
		
		try {
			(this->*it->second)();
		}
		catch (std::exception &e) {
			std::cerr << C_RED << e.what() << RESET << std::endl;
			if (_serverConfigs && !_serverConfigs->empty()) {
				for (std::vector<ServerConfig *>::iterator it = _serverConfigs->begin(); it != _serverConfigs->end(); it++)
					delete *it;
			}
			return (1);
		}
	}
	if (_serverConfigs->size() == 0) {
		std::cerr << C_RED << "Error: no server block is provided" << RESET << std::endl;
		return (1);
	}
	std::cout << C_GREEN << "End of configFile: no error" << RESET << std::endl;
	return (0);
}

int		ConfigParser::openFile(const std::string &filename) {
	std::string				buffer;
	std::string::iterator	it;

	errno = 0;
	if (filename.size() <= FILENAME_EXT_SIZE || filename.compare(filename.size() - FILENAME_EXT_SIZE, FILENAME_EXT_SIZE, FILENAME_EXT)) {
		std::cerr << C_RED << "Error: invalid extension, you're filename need to end with: \"" << FILENAME_EXT << "\"" << RESET << std::endl;
		return (1);
	}

	_configFile.open(filename.c_str());
	if (!_configFile.is_open()) {
		std::cerr << C_RED << "Unable to open the file " << filename << " :" << strerror(errno) << RESET << std::endl;
		return (errno);
	}
	
	return (0);
}

//TODO:
//- Add a default location if no one is given. OK
//- Ignore the end ligne with " #". OK
//- Add a line number for errors. : Use a ss to get the line, and overload getNextToken() to take int OK
//- Display a better error message.
//- check if the file end with ".conf". OK
//- Fix leaks when exception is thrown. Still some leaks : OK
//- Rename variables with wrong names
//- Read word by word the config file OK
//- Ajuste the size of bodySize in config file (1m = * 1'000'000), the parsin check the numbers of chars.
//- remove the setPath function in location OK
//- add default index.html if no one is provided OK
//- Combine "client_header_timeout" and "client_body_timeout" into "client_timeout" OK
//- Keep in mind that it may be necessary to remove upload_store
//- Make a logger with different level ?
//- unform some function (like setMaxBodySize, setClientTimeout, etc...)

/**
 * @biref get the next token in the config file and remove it from the file string
 * @return the token or an empty string if the end of the file has been reached
 */
int		ConfigParser::getNextToken(std::string &token) {
	_configFile >> token;
	if (_configFile.eof() || _configFile.fail())
		return (0);
	while (*token.begin() == '#') {
		_configFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //WARN: if the comment is the last word of the line , we skip the new line ???
		_configFile >> token;
		if (_configFile.eof() || _configFile.fail())
			return (0);
	}
	LOG_DEBUG(C_PINK << token << RESET);
	return (1);
}

int		ConfigParser::getNextToken(int &token) {
	std::string str;

	if (!getNextToken(str))
		return (0);
	std::stringstream ss(str);
	ss >> token;
	if (token < 0 || ss.fail())
		return (0);
	return (1);
}

bool	ConfigParser::thereIsDefaultLocation(void) const {
	std::vector<Location*> locations;

	if (!_currentServer)
		throw (std::logic_error("No server config to check for default location"));

	locations = _currentServer->getLocations();
	for (std::vector<Location *>::iterator it = locations.begin(); locations.end() != it; it++) {
		if ((*it)->getPath() == "/")
			return (true);
	}
	return (false);
}

bool	ConfigParser::thereIsIndex(void) const {
	if (!_currentLocation)
		throw (std::logic_error("No location to check for index"));

	if (_currentLocation->getIndexes().empty())
		return (false);
	return (true);
}

void	ConfigParser::processEndBraquetToken(void) {
	switch (_contextLevel) {
		case HTTP:
			delete (_http);
			_http = 0;
			break;
		case SERVER:
			if (!thereIsDefaultLocation()) {
				LOG_WARN("No default location found, create one");
				Location *location = new Location(*_currentServer, "/");
				location->addIndex("index.html");
				_currentServer->addLocation(location);
			}
			_currentServer = 0;
			break;
		case LOCATION:
			if (!thereIsIndex()) {
				LOG_WARN("No index found for location " << _currentLocation->getPath() << ", add default index \"index.html\"");
				_currentLocation->addIndex("index.html");
			}
			_currentLocation = 0;
			break;
		default:
			throw std::runtime_error("Unexcepted token \"}\"");
	}
	_contextLevel -= 1;
}

void	ConfigParser::processTokenHttp(void) {
	std::string	braquet;

	if (!getNextToken(braquet))
		throw (std::invalid_argument("fail to get the nextToken for \"http\""));
	if (braquet != "{" || _contextLevel != NONE || _http)
		throw (std::runtime_error("Unexcepted token after \"http\""));
	_http = new Http();
	_currentContext = _http;
	_contextLevel = HTTP;
	}

void	ConfigParser::processServerToken(void) {
		std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get the nextToken for \"server\""));
	if (nextToken != "{" || _contextLevel != HTTP || _currentServer)
		throw (std::runtime_error("Unexcepted token after \"server\""));
	_serverConfigs->push_back(new ServerConfig( *_http));
	_currentServer = *_serverConfigs->rbegin();
	_currentContext = _currentServer;
	_contextLevel = SERVER;
}

void	ConfigParser::processLocationToken(void) {
	std::string	path;
	std::string	braquet;

	if (!getNextToken(path) || !getNextToken(braquet))
		throw (std::invalid_argument("fail to get the nextToken for \"location\""));
	if (_currentLocation)
		throw std::runtime_error("Nested location is not allowed");

	if (braquet != "{" || _contextLevel!= SERVER || _currentLocation)
		throw std::runtime_error("Unexcepted token after \"Location\"");

	_currentLocation = new Location(*_currentServer, path);
	_currentContext = _currentLocation;
	_contextLevel = LOCATION;
	_currentServer->addLocation(_currentLocation);
}


/**
 * @detail client_timeout context: http, server
 */
void	ConfigParser::processClientTimeoutToken(void) {
	int			timeout;

	if (!getNextToken(timeout))
		throw (std::invalid_argument("fail to get the nextToken for \"cient_timeout\""));
	_currentContext->setClientTimeout(timeout);
	return ;
}

/**
 * @detail client_max_body_size context: http, server, location
 */
void	ConfigParser::processMaxBodySizeToken(void) {
	int			maxBodySize;

	if (!getNextToken(maxBodySize))
		throw (std::invalid_argument("fail to get the nextToken for \"max_body_size\""));
	_currentContext->setMaxBodySize(maxBodySize);
}

/**
 * @detail root context: http, server, location
 */
void	ConfigParser::processRootToken(void) {
	std::string nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get the nextToken for \"root\""));
	_currentContext->setRoot(nextToken);
	return;
}

/**
 * @detail listen context: server
 */
void	ConfigParser::processListenToken(void) {
		int 		port;

	if (!getNextToken(port))
		throw (std::invalid_argument("fail to get the nextToken for \"listen\""));
	_currentContext->setPort(port);
	return;
}

/**
 * @detail autoindex context: http, server, location
 * @detail only response possible on | off
 */
void	ConfigParser::processAutoindexToken(void) {
	std::string value;

	if (!getNextToken(value))
		throw (std::invalid_argument("fail to get the nextToken for \"autoindex\""));
	if (value == "on")
		_currentContext->setAutoindex(true);
	else if (value == "off")
		_currentContext->setAutoindex(false);
	else
		throw std::runtime_error("value " + value + " unrecognized after autoindex");
	return;
}

/**
 * @detail error_page context: http, server, location
 */
void	ConfigParser::processErrorPageToken(void) {
	int			code;
	std::string	pageToken;

	if (!getNextToken(code) || !getNextToken(pageToken))
		throw (std::invalid_argument("fail to get the nextToken for \"error_page\""));

	_currentContext->addErrorPage(code, pageToken);
}

/**
 * @detail server_name context: server
 */
void	ConfigParser::processServerNameToken(void) {
	std::string serverName;

	if (!getNextToken(serverName))
		throw (std::invalid_argument("fail to get the nextToken for \"server_name\""));
	_currentContext->setServerName(serverName);
}


void	ConfigParser::processAllowedMethodsToken(void) {
	std::string allowedMethods;

	if (!getNextToken(allowedMethods))
		throw (std::invalid_argument("fail to get the nextToken for \"allowed_methods\""));
	_currentContext->setAllowedMethods(allowedMethods);
}

void	ConfigParser::processIndexToken(void) {
	std::string index;

	if (!getNextToken(index))
		throw (std::invalid_argument("fail to get the nextToken for \"index\""));
	_currentContext->addIndex(index);
}

void	ConfigParser::processRedirectionToken(void) {
	int			code;
	std::string	redirectionPath;

	if (!getNextToken(code) || !getNextToken(redirectionPath))
		throw (std::invalid_argument("fail to get the nextToken for \"return\""));
	_currentContext->setRedireCode(code);
	_currentContext->setRedirePath(redirectionPath);
}

void	ConfigParser::processCgiPassToken(void) {
	std::string	extension;
	std::string	path;

	if (!getNextToken(extension) || !getNextToken(path))
		throw (std::invalid_argument("fail to get the nextToken for \"cgi_pass\""));
	if (*(extension.begin()) != '.')
		throw (std::invalid_argument("an extension need to start with a point"));
	_currentContext->addCgiExtension(extension, path);
}

