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
		int	openFile(std::string &filename);

		std::string	_configFile;
		std::string	_currentToken;
		int			_level;
};

#endif
