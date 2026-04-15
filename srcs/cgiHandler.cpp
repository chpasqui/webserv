#include "../include/Cgi.hpp"
#include "../include/Request.hpp"
#include "../include/ServerConfig.hpp"

#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
//https://www.cs.toronto.edu/~penny/teaching/csc309-01f/lectures/40/CGI.pdf

std::string intToString(int number){
	std::stringstream ss;
	ss << number;
	std::string newString;
	ss >> newString;
	return newString;
}

std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& tofind)
{
	std::map<std::string, std::string>::const_iterator it = header.find(tofind);
	if (it != header.end())
		return it->second;
	return "";
}

void	Cgi::buildCgiEnv(Request &request, const std::string& internalPath, const ServerConfig *config) {
	std::vector<std::string> env;
	const std::map<std::string, std::string>& header = request.getHeaders();

	env.push_back("REQUEST_METHOD=" + request.getMethod()); // GET POST DELETE
	env.push_back("QUERY_STRING=" + request.getQuery()); // query string must be defined as empty string if not found (treated by getHeaderVariable)
	env.push_back("SERVER_PROTOCOL=" + request.getVersion());

	env.push_back("CONTENT_TYPE=" + getHeaderVariable(header, "content-type"));
	if (request.getContentLength() != 0)
		env.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length")); // if and onlf if body.. need to check with charlotte
	env.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	this ->scriptname =  internalPath.substr(0, internalPath.find(".py") + 3);
	env.push_back("SCRIPT_NAME=" + this->scriptname); // need to split? not sure how i willr eceive it.... see wiht charlotte today
	env.push_back("PATH_INFO=" + internalPath.substr(scriptname.length())); // derived from the portion of the uri path following the part that identifies the script itself 
	
	
	std::string port = intToString(config->getPort());// I need the server port inside here 
	env.push_back("SERVER_PORT=" + port);
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	env.push_back("SERVER_NAME=" + std::string(hostname));
	for (size_t i = 0; i < env.size(); ++i)
		std::cout << GREEN << env[i] << RESET << std::endl;
}


void	Cgi::handleCgi(Request *request, std::string pathInfo, const ServerConfig *config) // charlotte needs to send this and then the path 
{
	std::cout << "ENTERED CGI FUNCTIOM" << std::endl;
	(void)request;
	if (!access(pathInfo.c_str(), X_OK)) { return; }
	std::cout << "the file at " << pathInfo << " exits!" << std::endl;
	buildCgiEnv(*request, pathInfo, config);
	// get the response information 
	// locate the program corresponding to the request
	// starts up the handling program (fork pipe, etc.)
	// feeds the request parameters to the child process via the environmental variabkes
	// handler executes the program (child program)
	// send back through the pipe to the parent back to the browser 
	// tzpicallz a browser
	// the child is over
	
	// in what can I coontain the env variable?
	// 

}
//copy of env TO DO

// if the method is POST, then i need to pass on the body to the CGI stdin from server -> tell charlotte



//  The server acts as an application gateway.  It receives the request
//    from the client, selects a CGI script to handle the request, converts
//    the client request to a CGI request, executes the script and converts
//    the CGI response into a response for the client.  When processing the
//    client request, it is responsible for implementing any protocol or
//    transport level authentication and security.  The server MAY also
//    function in a 'non-transparent' manner, modifying the request or
//    response in order to provide some additional service, such as media
//    type transformation or protocol reduction.

//CGI defines the abstract parameters, known as metavariables, which describe the client's request.


// If the request has a ./cgi-bin/ inside, then we identify the request as being CGI
// it comes with MEHOD URI (inside the uri is the cgi) and
// first I can check If i receive a directory or a file?
// 1. I need to create a chid process which will handle this --> first before I fork with a child, i need the env varuables
// od I need to creat a class for CGI?

// new process is created, inherits from its parent
// 	environment is passed from the parent to the child, its the way to send it 



// 1. the server receives the request but the URL does not map to a flat file but to a scripting area
//	server starts the script --> It creates a process and feeds it the information via the env
// 

// servrer scripts ar eprohrams that are run in the web server in response to request from 
// the clien t : these scripts profuce normal html as output which is fed back to the client if the clienn
// has requested an ordinary page... (Not sure i understand this part)
// javascript runs on the client side so it is not an exampe. 


    // CONTENT_LENGTH : It specifies the length of URL encoded data in bytes. OK
    // REMOTE_HOST : resolved host name of the client. OK
    // CONTENT_TYPE : It specifies the type of data as a MIME header. OK
    // QUERY_STRING : Gives information at the end of the URL after ? OK
    // REMOTE_ADDR : IP address of the client making the request.
    // METHOD: It gives the HTTP request method(GET or POST ). OK
    // SCRIPT_NAME : Name of the CGI script which is being run. 