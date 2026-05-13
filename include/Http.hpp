#ifndef HTTP_HPP
# define HTTP_HPP

#include <map>
#include <string>
#include <vector>
#include "AConfig.hpp"

class Http : public AConfig
{
	public:
		Http();
		virtual ~Http();
		//----Getter----//
		int							getClientTimeout(void) const;
		int							getMaxBodySize(void) const;
		bool						getAutoindex(void) const;
		std::string					getRoot(void) const;
		std::string					getAllowedMethods(void) const;
		std::vector<std::string>	getIndexes(void) const;
		std::map<int, std::string>	getErrorPages(void) const;
		
		//----Setter----//
		void						setClientTimeout(const int bodyTimeout);
		void						setMaxBodySize(const int maxBodySize);
		void						setAutoindex(const bool autoindex);
		void						setRoot(const std::string root);
		void						setAllowedMethods(const std::string allowedMethods);
		void						addIndex(const std::string index);
		void						addErrorPage(const int code, std::string page);

	private:
		int							_clientTimeout;
		int							_maxBodysize;
		bool						_autoindex;
		std::string					_root;
		std::string					_allowedMethods;
		std::vector<std::string>	_indexes;
		std::map<int, std::string>	_errorPages;
};

#endif
