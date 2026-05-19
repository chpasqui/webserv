#include "../../include/ServerConfig.hpp"
#include "../../include/Debug.hpp"
#include "../../include/color.hpp"
#include "../../include/Http.hpp"
#include "../../include/Location.hpp"


ServerConfig::ServerConfig() :
	_port(0),
	_serverName(""),
	_locations(0),
	_clientTimeout(0),
	_maxBodysize(0),
	_autoindex(false),
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexes(0),
	_errorPages() {
	LOG_DEBUG(C_CSTR << "ServerConfig: Default constructor called" << RESET);
	return ;
}

ServerConfig::ServerConfig(const Http &http) :
	_port(0),
	_serverName(""),
	_locations(0),
	_clientTimeout(http.getClientTimeout()),
	_maxBodysize(http.getMaxBodySize()),
	_autoindex(http.getAutoindex()),
	_root(http.getRoot()),
	_allowedMethods(http.getAllowedMethods()),
	_indexes(http.getIndexes()),
	_errorPages(http.getErrorPages()) {
	LOG_DEBUG(C_CSTR << "ServerConfig: Normal constructor called" << RESET);
	return ;
}

ServerConfig::~ServerConfig(){
	if (!_locations.empty())
		for (std::vector<Location *>::iterator it = _locations.begin(); it != _locations.end(); it++)
			delete (*it);
	_locations.clear();
	return ;
	LOG_DEBUG(C_DSTR << "ServerConfig: Destructor called" << RESET);
}

//----Setter---//

void ServerConfig::setPort(const int port) {
	_port = port;
}

void ServerConfig::setServerName(const std::string serverName) {
	_serverName = serverName;
}

void ServerConfig::addLocation(Location *location) {
	_locations.push_back(location);
}

void ServerConfig::setClientTimeout(int clientTimeout) {
	_clientTimeout = clientTimeout;
}

void ServerConfig::setMaxBodySize(int maxBodySize) {
	_maxBodysize = maxBodySize;
}

void ServerConfig::setAutoindex(bool autoindex) {
	_autoindex = autoindex;
}

void ServerConfig::setRoot(const std::string route) {
	_root = route;
}

void ServerConfig::setAllowedMethods(const std::string allowedMethods) {
	_allowedMethods = allowedMethods;
}

void ServerConfig::addIndex(const std::string index) {
	_indexes.push_back(index);
}

std::map<int, std::string> ServerConfig::getErrorPages(void) const
{
	return (_errorPages);
}

//----Getter----//

int	ServerConfig::getPort(void) const {
	return (_port);
}

std::string ServerConfig::getServerName(void) const {
	return (_serverName);
}

std::vector<Location *> ServerConfig::getLocations(void) const {
	return (_locations);
}

int ServerConfig::getClientTimeout(void) const {
	return (_clientTimeout);
}

int ServerConfig::getMaxBodySize(void) const {
	return (_maxBodysize);
}

bool ServerConfig::getAutoindex(void) const {
	return (_autoindex);
}

std::string ServerConfig::getRoute(void) const {
	return (_root);
}

std::string ServerConfig::getAllowedMethods(void) const {
	return (_allowedMethods);
}

std::vector<std::string> ServerConfig::getIndexes(void) const {
	return (_indexes);
}

void ServerConfig::addErrorPage(int code, std::string page) {
	_errorPages.insert(std::map<int, std::string>::value_type(code, page));
}

/**
 * @brief Returns the longest-prefix matching location for the given URL.
 * @return Pointer to matching Location, or NULL if none found.
 */
Location *ServerConfig::findLocation(const std::string &url) const
{
	Location *best = NULL;
	size_t bestLen = 0;

	std::vector<Location *> locs = getLocations();
	std::vector<Location *>::const_iterator it = locs.begin();
	for (; it != locs.end(); it++)
	{
		const std::string &prefix = (*it)->getPath();
		if (url.substr(0, prefix.size()) == prefix && prefix.size() > bestLen)
		{
			best = *it;
			bestLen = prefix.size();
		}
	}
	return best;
}

