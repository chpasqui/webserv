#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>

class Location
{
	public:
		Location();
		~Location();
	private:
		std::string path;
		int redirCode;
		std::string newPath;
		std::string dictionary;
};

#endif
