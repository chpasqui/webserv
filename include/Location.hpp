#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <map>
#include <string>
#include <vector>
#include "AConfig.hpp"

class ServerConfig;

class Location : public AConfig
{
	public:
		Location(const ServerConfig &serverConfig, const std::string path);
		virtual ~Location();
		//----Setter----//
		void						setRedireCode(const int redireCode);
		void						setRedirePath(const std::string redirePath);
		void						setUploadStore(const std::string uploadDir);
		void						addCgiExtension(const std::string extension, const std::string path);
		void						setBodyTimeout(const int bodyTimeout);
		void						setMaxBodySize(const int maxBodySize);
		void						setAutoindex(const bool autoindex);
		void						setRoot(const std::string root);
		void						setAllowedMethods(const std::string allowedMethods);
		void						addIndex(const std::string index);
		void						addErrorPage(const int code, const std::string page);
		//----Getter----//
		int							getRedirCode(void) const;
		std::string					getPath(void) const;
		std::string					getRedirPath(void) const;
		std::string					getUploadStore() const;
		std::map<std::string, std::string>	getCgiExtensions(void) const;
		int							getHeaderTimeout(void) const;
		int							getBodyTimeout(void) const;
		int							getMaxBodySize(void) const;
		int 						getPort(void) const;
		bool 						getAutoIndex(void) const;
		std::string					getServerName(void) const;
		std::string 				getRoot(void) const;
		std::string 				getAllowedMethods(void) const;
		std::vector<std::string> 	getIndexes(void) const;
		std::string					getErrorPage(int code) const;

	private:
		Location();

		int									_redireCode; //WARN: Waht if we have two return in the same location ?
		std::string							_path;
		std::string							_redirePath;
		std::string							_uploadStore;
		std::map<std::string, std::string>	_cgiExtensions;

		int									_headerTimeout;
		int									_bodyTimeout;
		int									_maxBodysize;
		int									_port;
		bool								_autoindex;
		std::string							_serverName;
		std::string							_root;
		std::string							_allowedMethods;
		std::vector<std::string>			_indexes;
		std::map<int, std::string>			_errorPages;
};

#endif
