#ifndef DEBUG_HPP
# define DEBUG_HPP

# include <iostream>
# include <string>
# include <map>
# include "color.hpp"

#ifndef LOG_LEVEL
# define LOG_LEVEL 0
#endif

enum DEBUG_LEVEL {
	ERR = 1,
	WARN = 2,
	INFO = 3,
	DEBUG = 4
};

# define LOG_DEBUG(log) if(LOG_LEVEL >= DEBUG) std::cout << C_PINK << log << RESET << std::endl
# define LOG_INFO(log) if(LOG_LEVEL >= INFO) std::cout << C_BLUE << log << RESET << std::endl
# define LOG_WARN(log) if(LOG_LEVEL >= WARN) std::cout << C_YELLOW << log << RESET << std::endl
# define LOG_ERR(code, ctx) if(LOG_LEVEL >= ERR) std::cerr << C_RED << "Error" << code << " : " << ctx << RESET << std::endl

class Debug {
	public:
		static void debugListHeader(std::map<std::string, std::string> &_headers);
	private:
		Debug();
		~Debug();
};

#endif
