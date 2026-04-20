#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"


#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>

/**
 * @brief Executes the CGI handler for the matched script.
 */
void Response::handleCGI(Request& request, char **envp){
    CGI cgi; //voir avec Josephine

    stat()
    int _statusCode = cgi.execute();
    if (_statusCode != 200){
        buildError();
        return ;
    }
    return ;
}
