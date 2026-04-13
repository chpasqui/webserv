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


// If the request has a ./cgi-bin/ inside, then we identify the request as being CGI
// it comes with MEHOD URI (inside the uri is the cgi) and
// first I can check If i receive a directory or a file?
// 1. I need to create a chid process which will handle this --> first before I fork with a child, i need the env varuables
// od I need to creat a class for CGI?

