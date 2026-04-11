#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

#include <string>

enum level {
	NONE,
	HTTP,
	SERVER,
	LOCATION
};

class ParseConfig {
	public:
		ParseConfig();
		ParseConfig(std::string filename);
		~ParseConfig();

		void	getNextTocken(void);
	private:
		std::string	configFile;
		std::string	currentToken;
		int			level;
};

#endif
