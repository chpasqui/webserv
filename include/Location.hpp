#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>

class Location{
	public:
		int getRedirectCode() const;
		std::string getNewPath() const;
		std::string getMethodAllowed() const;
		std::vector<std::string> getIndexes() const;
		bool getAutoIndex() const;

};

#endif