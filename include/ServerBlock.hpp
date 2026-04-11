#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include <string>
# include <vector>

class ServerBlock
{
	public:
		ServerBlock();
		~ServerBlock();
	private:
		int							_port;
		std::string					_rootDir;
		std::vector<std::string>	_dictionary;
};

#endif
