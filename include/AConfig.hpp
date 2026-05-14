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
		virtual void						setRedirePath(const std::string redirePath);
		virtual void						addCgiExtension(const std::string extension, const std::string path);
		virtual void						setClientTimeout(const int bodyTimeout);
		virtual void						setMaxBodySize(const int maxBodySize) = 0;
		virtual void						setServerName(const std::string serverName);
		virtual void						setPort(const int port);
		virtual void						setAutoindex(const bool autoindex) = 0;
		virtual void						setRoot(const std::string root) = 0;
		virtual void						setAllowedMethods(const std::string allowedMethods) = 0;
		virtual void						addIndex(const std::string index) = 0;
		virtual void						addErrorPage(const int code, const std::string page) = 0;

	protected:

	private:
		
};

#endif
