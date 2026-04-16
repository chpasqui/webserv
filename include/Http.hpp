#ifndef HTTP_HPP
# define HTTP_HPP

#include <map>
#include <string>
#include <vector>

class Http
{
	public:
		Http();
		~Http();
		//----Setter----//
		int							getHeaderTimeout(void) const;
		int							getBodyTimeout(void) const;
		int							getMaxBodySize(void) const;
		bool						getAutoIndex(void) const;
		std::string					getRoot(void) const;
		std::string					getAllowedMethods(void) const;
		std::vector<std::string>	getIndexs(void) const;
		std::map<int, std::string>	getErrorPages(void) const;
		//----Getter----//
		void						setHeaderTimeout(int headerTimeout);
		void						setBodyTimeout(int bodyTimeout);
		void						setMaxBodySize(int maxBodySize);
		void						setAutoIndex(bool autoindex);
		void						setRoot(std::string root);
		void						setAllowedMethods(std::string allowedMethods);
		void						addIndex(std::string index);
		void						addErrorPage(int code, std::string page);
	private:
		int							_headerTimeout;
		int							_bodyTimeout;
		int							_maxBodysize;
		bool						_autoindex;
		std::string					_root;
		std::string					_allowedMethods;
		std::vector<std::string>	_indexs;
		std::map<int, std::string>	_errorPages;
};

#endif
