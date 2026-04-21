#ifndef ICONFIG_HPP
# define ICONFIG_HPP

#include <string>
#include <vector>

class IConfig
{
	public:
		IConfig();
		~IConfig();
	
	protected:
		//----Setter----//
		virtual void						setRedireCode(const int redireCode) = 0;
		virtual void						setCgiEnable(const bool cgiEnable) = 0;
		virtual void						setPath(const std::string path) = 0;
		virtual void						setredirePath(const std::string redirePath) = 0;
		virtual void						addCgiExtension(const std::string extension) = 0;
		virtual void						setHeaderTimeout(int headerTimeout) = 0;
		virtual void						setBodyTimeout(int bodyTimeout) = 0;
		virtual void						setMaxBodySize(int maxBodySize) = 0;
		virtual void						setPort(const int port) = 0;
		virtual void						setAutoindex(const bool autoindex) = 0;
		virtual void						setRoot(const std::string root) = 0;
		virtual void						setAllowedMethods(const std::string allowedMethods) = 0;
		virtual void						addIndex(const std::string index) = 0;
		virtual void						addErrorPage(int code, std::string page) = 0;
		//----Getter----//
		virtual int							getRedirCode(void) const = 0;
		virtual bool						getCgiEnable(void) const = 0;
		virtual std::string					getPath(void) const = 0;
		virtual std::string					getRedirPath(void) const = 0;
		virtual std::vector<std::string>	getCgiExtensions(void) const = 0;
		virtual int							getHeaderTimeout(void) const = 0;
		virtual int							getBodyTimeout(void) const = 0;
		virtual int							getMaxBodySize(void) const = 0;
		virtual int 						getPort(void) const = 0;
		virtual bool 						getAutoIndex(void) const = 0;
		virtual std::string					getServerName(void) const = 0;
		virtual std::string 				getRoot(void) const = 0;
		virtual std::string 				getAllowedMethods(void) const = 0;
		virtual std::vector<std::string> 	getIndexes(void) const = 0;
		virtual std::string					getErrorPage(int code) const = 0;
		virtual std::string					getUploadStore() const = 0;

	private:
		
};

#endif
