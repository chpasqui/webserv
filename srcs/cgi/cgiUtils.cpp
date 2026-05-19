#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>

std::string getHeaderVariable(const std::map<std::string, std::string> &header, const std::string &key)
{
	std::map<std::string, std::string>::const_iterator it = header.find(key);
	if (it != header.end())
		return it->second;
	return "";
}

// getheader in response

// const std::string& Request::getHeader(std::string key) const{
//     static const std::string empty = "";
//     std::map<std::string, std::string>::const_iterator it = _headers.begin();
// 	for(; it != _headers.end(); it++)
//         if (it->first == key)
//             return it->second;
//     return empty;
// } already exists call this functio nn

std::string parseShebang(const std::string &internalPath)
{
	std::ifstream filestream(internalPath.c_str());
	std::string line;
	getline(filestream, line);
	if (line.substr(0, 2) != "#!")
	{
		return "";
	}

	if (line.find("env") != std::string::npos)
	{
		size_t spacepos = line.find(' ');
		std::string interpreterName = line.substr(spacepos + 1);
		return "/usr/bin/" + interpreterName; // /usr/bin/python3
	}

	return line;
}

std::string resolveHostname()
{
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	return (std::string(hostname));
}

std::string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	std::string newString;
	ss >> newString;
	return newString;
}

