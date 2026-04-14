#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>

class ServerConfig;

class Location
{
	public:
		Location(ServerConfig serverConfig);
		~Location();

		void	setRoot(std::string root);
		void	getAllowedMethods(std::string allowedMethods);
		void	setPath(std::string path);
		void	setredirePath(std::string redirePath);
		void	setAutoindex(bool autoindex);
		void	setCgiEnable(bool cgiEnable);
		void	setRedireCode(int redireCode);

	private:
		Location();

		std::string	_root;
		std::string	_allowedMethods;
		bool		_autoindex;

//INFO: only in location
		std::string	_path;
		bool		_cgiEnable;
		int			_redireCode;
		std::string	_redirePath;
};

#endif
