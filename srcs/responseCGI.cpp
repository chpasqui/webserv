#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"


#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>

void Response::handleCGI(Request& request, char **envp){
    CGI cgi; //voir avec Josephine
    int _statusCode = cgi.execute();
    if (_statusCode != 200){
        buildError();
        return ;
    }
    return ;
}
