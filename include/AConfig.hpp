#ifndef ICONFIG_HPP
# define ICONFIG_HPP

#include <string>

class AConfig
{
	public:
		AConfig();
		~AConfig();

		//WARN: Move all this fonction in protected later
		//----Setter----//
		virtual void						setRedireCode(const int redireCode);
		virtual void						setCgiEnable(const bool cgiEnable);
		virtual void						setPath(const std::string path);
		virtual void						setredirePath(const std::string redirePath);
		virtual void						addCgiExtension(const std::string extension);
		virtual void						setHeaderTimeout(const int headerTimeout);
		virtual void						setBodyTimeout(const int bodyTimeout);
		virtual void						setMaxBodySize(const int maxBodySize);
		virtual void						setPort(const int port);
		virtual void						setAutoindex(const bool autoindex);
		virtual void						setRoot(const std::string root);
		virtual void						setAllowedMethods(const std::string allowedMethods);
		virtual void						addIndex(const std::string index);
		virtual void						addErrorPage(const int code, const std::string page);
	
	protected:
		//----Getter----//
		// virtual int							getRedirCode(void) const = 0;
		// virtual bool						getCgiEnable(void) const = 0;
		// virtual std::string					getPath(void) const = 0;
		// virtual std::string					getRedirPath(void) const = 0;
		// virtual std::vector<std::string>	getCgiExtensions(void) const = 0;
		// virtual int							getHeaderTimeout(void) const = 0;
		// virtual int							getBodyTimeout(void) const = 0;
		// virtual int							getMaxBodySize(void) const = 0;
		// virtual int 						getPort(void) const = 0;
		// virtual bool 						getAutoIndex(void) const = 0;
		// virtual std::string					getServerName(void) const = 0;
		// virtual std::string 				getRoot(void) const = 0;
		// virtual std::string 				getAllowedMethods(void) const = 0;
		// virtual std::vector<std::string> 	getIndexes(void) const = 0;
		// virtual std::string					getErrorPage(int code) const = 0;
		// virtual std::string					getUploadStore() const = 0;

	private:
		
};

#endif
