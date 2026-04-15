#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>

class Location{
	public:
		int getRedirectCode() const;
		std::string getRoot() const;
		std::string getPath() const;
		std::string getNewPath() const;
		std::string getMethodAllowed() const;
		std::vector<std::string> getIndexes() const;
		bool getAutoIndex() const;
		bool getCgiEnabled() const;
		bool getUploadAllowed() const;
		std::string getUploadStore() const;
		std::vector<std::string> getCgiExtensions() const;

};

#endif