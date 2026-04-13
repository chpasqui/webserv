#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>

class Location{
	public:
		int getRedirectCode() const;
		std::string getNewPath() const;
		std::string getMethodAllowed() const;

};

#endif