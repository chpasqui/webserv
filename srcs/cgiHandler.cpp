#include "Request.hpp"
#include "Cgi.hpp"

//https://www.cs.toronto.edu/~penny/teaching/csc309-01f/lectures/40/CGI.pdf

/**
 * @file cgiHandler.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

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

void	Cgi::handleCgi(Request *request, std::string cgiPath)
{
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


// 1. the server receives the request but the URL does not map to a flat file but to a scripting area
//	server starts the script --> It creates a process and feeds it the information via the env
// 

// servrer scripts ar eprohrams that are run in the web server in response to request from 
// the clien t : these scripts profuce normal html as output which is fed back to the client if the clienn
// has requested an ordinary page... (Not sure i understand this part)
// javascript runs on the client side so it is not an exampe. 