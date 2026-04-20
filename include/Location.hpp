#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <map>
#include <string>
#include <vector>

class ServerConfig;

class Location
{
	public:
		Location(const ServerConfig &serverConfig);
		~Location();
		//----Setter----//
		void						setRedireCode(const int redireCode);
		void						setCgiEnable(const bool cgiEnable);
		void						setPath(const std::string path);
		void						setredirePath(const std::string redirePath);
		void						addCgiExtension(const std::string extension);
		void						setHeaderTimeout(int headerTimeout);
		void						setBodyTimeout(int bodyTimeout);
		void						setMaxBodySize(int maxBodySize);
		void						setPort(const int port);
		void						setAutoindex(const bool autoindex);
		void						setRoot(const std::string root);
		void						setAllowedMethods(const std::string allowedMethods);
		void						addIndex(const std::string index);
		void						addErrorPage(int code, std::string page);
		//----Getter----//
		int							getRedirCode(void) const;
		bool						getCgiEnable(void) const;
		std::string					getPath(void) const;
		std::string					getRedirPath(void) const;
		std::vector<std::string>	getCgiExtensions(void) const;
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
		std::string					getUploadStore() const;

	private:
		Location();

		int							_redireCode;
		bool						_cgiEnable;
		std::string					_path;
		std::string					_redirePath;
		std::vector<std::string>	_cgiExtension;
		std::string					_uploadStore;

		int							_headerTimeout;
		int							_bodyTimeout;
		int							_maxBodysize;
		int							_port; //OPTIM: Location probably doesn't need that
		bool						_autoindex;
		std::string					_serverName; //OPTIM: Location probably doesn't need that
		std::string					_root;
		std::string					_allowedMethods;
		std::vector<std::string>	_indexes;
		std::map<int, std::string>	_errorPages;
};

#endif
