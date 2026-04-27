#ifndef ICONFIG_HPP
# define ICONFIG_HPP

#include <string>

class AConfig
{
	public:
		AConfig();
		~AConfig();

		//----Setter----//
		virtual void						setRedireCode(const int redireCode);
		virtual void						setCgiEnable(const bool cgiEnable);
		virtual void						setPath(const std::string path);
		virtual void						setRedirePath(const std::string redirePath);
		virtual void						addCgiExtension(const std::string extension);
		virtual void						setHeaderTimeout(const int headerTimeout);
		virtual void						setBodyTimeout(const int bodyTimeout);
		virtual void						setMaxBodySize(const int maxBodySize);
		virtual void						setServerName(const std::string serverName);
		virtual void						setPort(const int port);
		virtual void						setAutoindex(const bool autoindex);
		virtual void						setRoot(const std::string root);
		virtual void						setAllowedMethods(const std::string allowedMethods);
		virtual void						addIndex(const std::string index);
		virtual void						addErrorPage(const int code, const std::string page);

	protected:

	private:
		
};

#endif
