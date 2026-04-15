#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include <vector>

class Http
{
	public:
		Http();
		~Http();
		//----Setter----//
		bool						getAutoIndex(void) const;
		std::string					getRoot(void) const;
		std::string					getAllowedMethods(void) const;
		std::vector<std::string>	getIndexs(void) const;
		//----Getter----//
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
