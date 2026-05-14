#ifndef DEBUG_HPP
# define DEBUG_HPP

# include <string>
# include <map>

# define LOG 0

enum DEBUG_LEVEL {
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERR = 4
};

class Debug {
	public:
		Debug();
		~Debug();

		static void log(const std::string &msg, int level);
		static void log(const std::string &msg, const std::string &ctx);
		void debugListHeader(std::map<std::string, std::string> &_headers);
	private:
};


//# ifdef DEBUG
// #  define LOG(msg) std::cerr << msg << std::endl
// #  define LOG_ERR(code, ctx) std::cerr << "Error " << code << " : " << ctx << std::endl
// # else
// #  define LOG(msg) (void)0
// #  define LOG_ERR(code, ctx) (void)0
// # endif

#endif
