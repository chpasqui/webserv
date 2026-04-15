#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include <vector>

class HTTP
{
	public:
		HTTP();
		~HTTP();

		bool						getAutoIndex(void) const;
		std::string					getRoot(void) const;
		std::string					getAllowedMethods(void) const;
		std::vector<std::string>	getIndex(void) const;

		void						setAutoIndex(bool autoindex);
		void						setRoot(std::string root);
		void						setAllowedMethods(std::string allowedMethods);
		void						addIndex(std::string index);
	private:
		// int							_headerTimeout;
		// int							_bodyTimeout;
		// int							_maxBodysize;
		bool						_autoindex;
		std::string					_root;
		std::string					_allowedMethods;
		std::vector<std::string>	_indexs;
};

#endif
