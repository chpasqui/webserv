#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"
#include "../include/Cgi.hpp"
#include "../include/Request.hpp"


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>

#define WRITE_END 1
#define READ_END 0

// /**
//  * @brief Executes the CGI handler for the matched script.
//  */
// void Response::handleCGI(Request& request){
//     CGI cgi; //voir avec Josephine

//     stat()
//     int _statusCode = cgi.execute();
//     if (_statusCode != 200){
//         buildError();
//         return ;
//     }
//     return ;
// }


