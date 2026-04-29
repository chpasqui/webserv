#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>

std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& key) {
	std::map<std::string, std::string>::const_iterator it = header.find(key);
	if (it != header.end())
		return it->second;
	return "";
}

std::string parseShebang() {
	std::ifstream filestream("/www/cgi-bin/hello.py");
	std::string line;
	getline(filestream, line);
	std::cout << "line contains " << line << std::endl;
	if (line.substr(0,2) != "#!")
	{
		std::cout << "CASE 1" << std::endl;
		return "";
	}

	if (line.find("env") != std::string::npos) {
		std::cout << "CASE 2" << std::endl;
		size_t spacepos = line.find(' ');
		std::string interpreterName = line.substr(spacepos + 1);
		return "/usr/bin/" + interpreterName; // /usr/bin/python3
	}
	std::cout << "CASE 3" << std::endl;
	
	return line;
}

std::string resolveHostname() {
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	return (std::string(hostname));
}

std::string intToString(int number) {
	std::stringstream ss;
	ss << number;
	std::string newString;
	ss >> newString;
	return newString;
}
