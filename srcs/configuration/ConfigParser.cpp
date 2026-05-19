#include "../../include/ConfigParser.hpp"
#include <errno.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include "../../include/Http.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Location.hpp"
#include "../../include/color.hpp"
#include "../../include/Debug.hpp"

ConfigParser::ConfigParser() :
	_configFile(""),
	_currentLine(""),
	_contextLevel(0),
	_lineNumber(0),
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
	_currentLine(""),
	_contextLevel(NONE),
	_lineNumber(0),
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

	try {
		while (true)
		{
			getNextToken(token);
			it = _dictionary.find(token);
			if (it == _dictionary.end()) {
				std::cerr << C_RED << "Error: Unknow token \"" << token << "\" on line : " << _lineNumber << RESET  << std::endl;
				return (1);
			}
			(this->*it->second)();
		}
	}
	catch (ConfigParser::EndOfConfigFile &e) {
		LOG_INFO(e.what());
	}
	catch (std::exception &e) {
		std::cerr << C_RED << "Error: "<< e.what() << " on line : " << _lineNumber << RESET << std::endl;
		if (_serverConfigs && !_serverConfigs->empty()) {
			for (std::vector<ServerConfig *>::iterator it = _serverConfigs->begin(); it != _serverConfigs->end(); it++)
				delete *it;
		}
		return (1);
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
//- Fix leaks when exception is thrown. Still some leaks : OK
//- Ajuste the size of bodySize in config file (1m = * 1'000'000), the parsin check the numbers of chars.

/**
 * @brief get the next token in the config file and remove it from the file string
 * @return the token or an empty string if the end of the file has been reached
 */
void	ConfigParser::getNextToken(std::string &token) {
	getNextLine();
	size_t pos = _currentLine.find_first_not_of(" \t\f\r\v\n");
	_currentLine.erase(0, pos);
	pos = _currentLine.find_first_of(" \t\f\r\v\n");
	token = _currentLine.substr(0, pos);
	_currentLine.erase(0, pos);
	LOG_DEBUG("Token is: \"" << token << "\"");
}

void ConfigParser::getNextLine(void) {
	while (_currentLine.empty() || _currentLine.find_first_not_of(" \t\f\r\v\n") == std::string::npos || _currentLine.find("#") != std::string::npos) {
		if(!getline(_configFile, _currentLine))
			throw (ConfigParser::EndOfConfigFile());
		_lineNumber++;
		
		if (*_currentLine.begin() == '#') {
			if(!getline(_configFile, _currentLine))
				throw (ConfigParser::EndOfConfigFile());
			_lineNumber++;
			continue ;
		}

		size_t pos = _currentLine.find(" #");
		if (pos != std::string::npos) {
			_currentLine.erase(pos);
		}
	}
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
				LOG_INFO("No default location found, create one");
				Location *location = new Location(*_currentServer, "/");
				location->addIndex("index.html");
				_currentServer->addLocation(location);
			}
			_currentServer = 0;
			break;
		case LOCATION:
			if (!thereIsIndex()) {
				LOG_INFO("No index found for location " << _currentLocation->getPath() << ", add default index \"index.html\"");
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

	getNextToken(braquet);
	if (braquet != "{" || _contextLevel != NONE || _http)
		throw (std::runtime_error("Unexcepted token after \"http\""));
	_http = new Http();
	_currentContext = _http;
	_contextLevel = HTTP;
	}

void	ConfigParser::processServerToken(void) {
		std::string	nextToken;

	getNextToken(nextToken);
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

	getNextToken(path);
	getNextToken(braquet);
	if (_currentLocation)
		throw std::runtime_error("Nested location is not allowed");
	if (braquet != "{" || _contextLevel!= SERVER)
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
	std::string	timeout;

	getNextToken(timeout);

	int n = ConfigParser::strToInt(timeout);
	_currentContext->setClientTimeout(n);
}

/**
 * @detail client_max_body_size context: http, server, location
 */
void	ConfigParser::processMaxBodySizeToken(void) {
	std::string	maxBodySize;

	getNextToken(maxBodySize);
	
	int n = ConfigParser::strToInt(maxBodySize);
	_currentContext->setMaxBodySize(n);
}

/**
 * @detail root context: http, server, location
 */
void	ConfigParser::processRootToken(void) {
	std::string nextToken;

	getNextToken(nextToken);
	_currentContext->setRoot(nextToken);
}

/**
 * @detail listen context: server
 */
void	ConfigParser::processListenToken(void) {
	std::string	port;

	getNextToken(port);

	int n = ConfigParser::strToInt(port);
	_currentContext->setPort(n);
}

/**
 * @detail autoindex context: http, server, location
 * @detail only response possible on | off
 */
void	ConfigParser::processAutoindexToken(void) {
	std::string value;

	getNextToken(value);
	if (value == "on")
		_currentContext->setAutoindex(true);
	else if (value == "off")
		_currentContext->setAutoindex(false);
	else
		throw std::runtime_error("value " + value + " unrecognized after autoindex");
}

/**
 * @detail error_page context: http, server, location
 */
void	ConfigParser::processErrorPageToken(void) {
	std::string	code;
	std::string	pageToken;

	getNextToken(code);
	getNextToken(pageToken);

	int n = ConfigParser::strToInt(code);
	_currentContext->addErrorPage(n, pageToken);
}

/**
 * @detail server_name context: server
 */
void	ConfigParser::processServerNameToken(void) {
	std::string serverName;

	getNextToken(serverName);
	_currentContext->setServerName(serverName);
}


void	ConfigParser::processAllowedMethodsToken(void) {
	std::string allowedMethods;

	getNextToken(allowedMethods);
	_currentContext->setAllowedMethods(allowedMethods);
}

void	ConfigParser::processIndexToken(void) {
	std::string index;

	getNextToken(index);
	_currentContext->addIndex(index);
}

void	ConfigParser::processRedirectionToken(void) {
	std::string	code;
	std::string	redirectionPath;

	getNextToken(code);
	getNextToken(redirectionPath);

	int n = ConfigParser::strToInt(code);
	_currentContext->setRedireCode(n);
	_currentContext->setRedirePath(redirectionPath);
}

void	ConfigParser::processCgiPassToken(void) {
	std::string	extension;
	std::string	path;

	getNextToken(extension);
	getNextToken(path);
	if (*(extension.begin()) != '.')
		throw (std::invalid_argument("an extension need to start with a point"));
	_currentContext->addCgiExtension(extension, path);
}

int	ConfigParser::strToInt(const std::string &str) {
	int nb;
	std::stringstream ss(str);

	ss >> nb;
	if (ss.fail() || nb < 0)
		throw(std::invalid_argument("invalid number"));
	return (nb);
}

const char *ConfigParser::EndOfConfigFile::what(void) const throw() {
	return ("end of config file");
}

