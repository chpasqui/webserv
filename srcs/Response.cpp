#include "../include/Response.hpp"
#include "../include/statusCode.hpp"

Response::Response() :
    _statusCode(0),
    _reasonPhrase(""),
    _body("") {}

Response::~Response(){}

//Step1: verifier que le request status code soit != 0, if yes buildError(statusCode)
//Step2 : findlocation verifier quil existe ? if no 404
//Step3: y a t il une redirection ? if yes stop et buildError(300);
//Step4: la methode est elle authorise ? if no stop buildError(405);
//Step5: est ce que c'est un CGI ? if yes execute CGI et return;
//Step6: gerer les requests en fonction de la methode
void Response::build(Request& request, ServerConfig *config){
    if (request.getStatusCode() != 0)
        this->_statusCode = request.getStatusCode()
        buildError();
    if (isCGI())
        handleCGI();
    handleStaticRequest(request, config);
}

void Response::handleStaticRequest(Request& request, ServerConfig *config){
    if (request.getMethod() == "GET")
        handleGetMethod(request);
    if (request.getMethod() == "POST")
        handlePostMethod(request);
    else
        handleDeleteMethod(request);
}

//Step1: define type of uri (is it a directory or a file)
//Step2: if it is a dir but doesnt end with / ??? -> error
void Response::handleGetMethod(Request& request){
// if (url type == file)
    //returnRequestedFile()
// else
    //if (!request.geturl.end == '/')
        //buildError(301);
    //else
        //if index ? (i dont understand) --> check root
            //returnindex
        //else
            //if(config->location->autoindex == "off")
                //builderror(403);
            //else
                //return autoindex of dir
}

void Response::handlePostMethod(Request& request){
//if config->location->allowedmethod.find("POST")
    //uploadfile
//else
    //buildError(403)
}

void Response::handleDeleteMethod(Request& request){}

std::string findLocation(){
    //trouver si le URI correspond a une des locations
    //mettre cette fonction dans ServerConfig
    //renvoyer le nom de la location qui correpond le plus a la URI
    //est ce quil faut que jaggrege le root path ici 
}

void Response::buildError(){
    //renvoie la page html en fonction de la map 
    //mais est ce que jutilise cette fonction pour 
}