#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <vector>
#include <string>
class ServerConfig;

std::vector<ServerConfig> parseConfigFile(std::string configFile);

#endif
