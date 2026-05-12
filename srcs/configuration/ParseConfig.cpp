#include "../../include/ParseConfig.hpp"
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
	_configFile(""),
	_lineNumber(0),
	_contextLevel(0),
	_contextConfig(0),
	_http(0),
	_currentConfig(0),
	_location(0),
	_serverConfigs(0),
	_dictionary() {
	LOG(C_CSTR << "ParseConfig: Default constructor called" << RESET);
}

ParseConfig::ParseConfig(std::vector<ServerConfig *> *serverConfigs) :
	_configFile(""),
	_lineNumber(1),
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
	if (_http)
		delete (_http);
	_configFile.close();
	LOG(C_DSTR << "ParseConfig: Destructor called" << RESET);
}

int	ParseConfig::parsingConfigFile(const std::string &filename) {
	std::string														token;
	std::map<std::string, void (ParseConfig::*)(void)>::iterator	it;

	errno = 0;
	if (openFile(filename) != 0)
		return (1);

	while (true)
	{
		if (!getNextToken(token))
			break;
		it = _dictionary.find(token);
		if (it == _dictionary.end()) {
			std::cerr << C_RED << token << ": Unknow token on line: " << _lineNumber << RESET  << std::endl;
			return (1);
		}
		
		try {
			(this->*it->second)();
		}
		catch (std::exception &e) {
			std::cerr << C_RED << e.what() << "on line: " << _lineNumber << RESET << std::endl;
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

int		ParseConfig::openFile(const std::string &filename) {
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
//- Add a line number for errors. : Use a ss to get the line, and overload getNextToken() to take int
//- Display a better error message.
//- check if the file end with ".conf". OK
//- Fix leaks when exception is thrown. Still some leaks : OK
//- Rename variables with wrong names
//- Read word by word the config file OK
//- Ajuste the size of bodySize in config file (1m = * 1'000'000), the parsin check the numbers of chars.
//- remove the setPath function in location OK
//- add default index.html if no one is provided OK

/**
 * @biref get the next token in the config file and remove it from the file string
 * @return the token or an empty string if the end of the file has been reached
 */
int ParseConfig::getNextToken(std::string &token) {
	if (_line.eof()) {
		//std::cout << "DEBUG: line is empty" << std::endl;
		if (getNextLine() == 1)
			return (1);
	}

	_line >> token;
	if (_line.fail() || _line.tellp() == 0)
		return (0);

	LOG(C_PINK << "Token: " << token << RESET);
	return (1);
}

int ParseConfig::getNextToken(int &token) {
	if (_line.end) {
		//std::cout << "DEBUG: line is empty" << std::endl;
		if (getNextLine() == 1)
			return (1);
	}

	_line >> token;
	if (_line.fail() || _line.tellp() == 0)
		return (0);

	LOG(C_PINK << "Token: " << token << RESET);
	return (1);
}

int ParseConfig::getNextLine(void) {
	std::string	str;
	ssize_t		pos = 0;

	while (1) {
		_line.clear();
		if (!getline(_configFile, str))
			return (1);
		_lineNumber++;
		std::cout << C_PINK << "line: " << str << RESET << std::endl;
		if (*str.begin() == '#')
			continue ;
		if ((pos = str.find(" #", 0) != std::string::npos)) {
			std::cout << C_PINK << "pos is " << pos << RESET << std::endl;
			str = str.substr(0, pos);
		}
		if (str.find_first_not_of(" \f\n\r\t\v") == std::string::npos)
			continue ;
		break;
	}

	_line << str;
	return (0);
}

bool	ParseConfig::thereIsDefaultLocation(void) const {
	std::vector<Location*> locations;

	if (!_currentConfig)
		throw (std::logic_error("No server config to check for default location"));

	locations = _currentConfig->getLocations();
	for (std::vector<Location *>::iterator it = locations.begin(); locations.end() != it; it++) {
		if ((*it)->getPath() == "/")
			return (true);
	}
	return (false);
}

bool	ParseConfig::thereIsIndex(void) const {
	if (!_location)
		throw (std::logic_error("No location to check for index"));

	if (_location->getIndexes().empty())
		return (false);
	return (true);
}

void	ParseConfig::processEndBraquetToken(void) {
	switch (_contextLevel) {
		case HTTP:
			delete (_http);
			_http = 0;
			break;
		case SERVER:
			if (!thereIsDefaultLocation()) {
				LOG(C_PINK << "No default location found, create one" << RESET);
				_currentConfig->addLocation(new Location(*_currentConfig, "/"));
			}
			_currentConfig = 0;
			break;
		case LOCATION:
			if (!thereIsIndex()) {
				LOG(C_PINK << "No index found for location " << _location->getPath() << ", add default index \"index.html\"" << RESET);
				_location->addIndex("index.html");
			}
			_location = 0;
			break;
		default:
			throw std::runtime_error("Unexcepted token \"}\"");
	}
	_contextLevel -= 1;
}

void	ParseConfig::processTokenHttp(void) {
	std::string	braquet;

	if (!getNextToken(braquet))
		throw (std::invalid_argument("fail to get a token"));
	if (braquet != "{" || _contextLevel != NONE || _http)
		throw (std::runtime_error("Unexcepted token after \"http\""));
	_http = new Http();
	_contextConfig = _http;
	_contextLevel = HTTP;
	}

void	ParseConfig::processServerToken(void) {
		std::string	nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	if (nextToken != "{" || _contextLevel != HTTP || _currentConfig)
		throw (std::runtime_error("Unexcepted token after \"server\""));
	_serverConfigs->push_back(new ServerConfig( *_http));
	_currentConfig = *_serverConfigs->rbegin();
	_contextConfig = _currentConfig;
	_contextLevel = SERVER;
}

void	ParseConfig::processLocationToken(void) {
	std::string	path;
	std::string	braquet;

	if (!getNextToken(path) || !getNextToken(braquet))
		throw (std::invalid_argument("fail to get a token"));
	if (_location)
		throw std::runtime_error("Nested location is not allowed");

	if (braquet != "{" || _contextLevel!= SERVER || _location)
		throw std::runtime_error("Unexcepted token after \"Location\"");

	_location = new Location(*_currentConfig, path);
	_contextConfig = _location;
	_contextLevel = LOCATION;
	_currentConfig->addLocation(_location);
}

/**
 * @detail client_header_timeout context: http, server
 */
void	ParseConfig::processHeaderTimeoutToken(void) {
	int			timeout;

	if (!getNextToken(timeout))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setHeaderTimeout(timeout);
}

/**
 * @detail client_body_timeout context: http, server, location
 */
void	ParseConfig::processBodyTimeoutToken(void) {
	int			timeout;

	if (!getNextToken(timeout))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setBodyTimeout(timeout);
	return ;
}

/**
 * @detail client_max_body_size context: http, server, location
 */
void	ParseConfig::processMaxBodySizeToken(void) {
	int			maxBodySize;

	if (!getNextToken(maxBodySize))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setMaxBodySize(maxBodySize);
}

/**
 * @detail root context: http, server, location
 */
void	ParseConfig::processRootToken(void) {
	std::string nextToken;

	if (!getNextToken(nextToken))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setRoot(nextToken);
	return;
}

/**
 * @detail listen context: server
 */
void	ParseConfig::processListenToken(void) {
		int 		port;

	if (!getNextToken(port))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setPort(port);
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
		_contextConfig->setAutoindex(true);
	else if (value == "off")
		_contextConfig->setAutoindex(false);
	else
		throw std::runtime_error("value " + value + " unrecognized after autoindex");
	return;
}

/**
 * @detail error_page context: http, server, location
 */
void	ParseConfig::processErrorPageToken(void) {
	int			code;
	std::string	pageToken;

	if (!getNextToken(code) || !getNextToken(pageToken))
		throw (std::invalid_argument("fail to get a token"));

	_contextConfig->addErrorPage(code, pageToken);
}

/**
 * @detail server_name context: server
 */
void	ParseConfig::processServerNameToken(void) {
	std::string serverName;

	if (!getNextToken(serverName))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setServerName(serverName);
}


void	ParseConfig::processAllowedMethodsToken(void) {
	std::string allowedMethods;

	if (!getNextToken(allowedMethods))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setAllowedMethods(allowedMethods);
}

void	ParseConfig::processIndexToken(void) {
	std::string index;

	if (!getNextToken(index))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->addIndex(index);
}

void	ParseConfig::processRedirectionToken(void) {
	int			code;
	std::string	redirectionPath;

	if (!getNextToken(code) || !getNextToken(redirectionPath))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setRedireCode(code);
	_contextConfig->setRedirePath(redirectionPath);
}

void	ParseConfig::processCgiPassToken(void) {
	std::string	extension;
	std::string	path;

	if (!getNextToken(extension) || !getNextToken(path))
		throw (std::invalid_argument("fail to get a token"));
	if (*(extension.begin()) != '.')
		throw (std::invalid_argument("an extension need to start with a point"));
	_contextConfig->addCgiExtension(extension, path);
}

void	ParseConfig::processUploadStoreToken(void) {
	std::string uploadStorePath;

	if (!getNextToken(uploadStorePath))
		throw (std::invalid_argument("fail to get a token"));
	_contextConfig->setUploadStore(uploadStorePath);
}

