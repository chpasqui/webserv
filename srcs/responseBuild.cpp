#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"

//Step1: verifier que le request status code soit != 0, if yes buildError(statusCode) -- OK
//Step2 : findlocation verifier quil existe ? if not utilise le root du server -- OK
//Step3: y a t il une redirection ? if yes stop et buildError(300); -- OK
//Step4: la methode est elle authorise ? if no stop buildError(405);
//Step5: est ce que c'est un CGI ? if yes execute CGI et return;
//Step6: gerer les requests en fonction de la methode
void Response::build(Request& request, ServerConfig *config){
    if (request.getStatusCode() != 0){
        _statusCode = request.getStatusCode();
        buildError();
        return ;
    }
    //a regarder avec Hayden
    Location* loc = findLocation(request.getPath(), config); //si on met dans serverConfig config->findLocation
    _internalPath = buildFileSystemPath(loc, request.getPath(), config);
    if (loc->getRedirectCode != 0){ //du coup besoin dún booleen
        _statusCode = loc->getRedirectCode();
        _headers["Location"] = loc->getNewPath();
        return ;
    }
    if (!methodAllowed(loc, request.getMethod())){
        _statusCode = 405;
        buildError();
        return ;
    }
    //voir avec Josephine
    if (isCGI(_internalPath, loc)){
        handleCGI(???);
        return;
    }
    handleMethod(request, config);
}

void Response::handleMethod(Request& request, ServerConfig *config){
    if (request.getMethod() == "GET")
        handleGetMethod(request);
    else if (request.getMethod() == "POST")
        handlePostMethod(request);
    else
        handleDeleteMethod(request);
}

//Step1: trouver si le URI correspond a une des locations
//Step2: mettre cette fonction dans ServerConfig
//Step3: renvoyer le nom de la location qui correpond le plus a la URI
//Step4: est ce quil faut que jaggrege le root path ici 
Location* Response::findLocation(const std::string& url, ServerConfig* config){
    Location *best = nullptr;
    size_t bestLen = 0;

    std::vector<Location>& locs = config->getLocations(); // voir ac hayden cette histoire de vecteur
    std::vector<Location>::iterator it = locs.begin();
    for (; it != locs.end(); it++){
        const std::string& prefix = it->getPath();
        if (url.substr(0, prefix.size()) == prefix && prefix.size() > bestLen){
            best = &(*it);
            bestLen = prefix.size();
        }
    }
    return best;
}

std::string Response::buildFileSystemPath(Location* loc, const std::string& url, ServerConfig* config){
    std::string root;
    if (loc == nullptr || loc->getRoot().empty()) // est ce que y a root dans location oui ou non
        return config->getRoot() + path;
    
    std::string remain = url.substr(loc->getPath().size());
    return config->getRootPath() + "/" + remain; 

}

bool Response::MethodAllowed(Location *loc, const std::string& method){
    std::string methodAllowed = loc->getMethodAllowed();
    if (methodAllowed.find(method) != std::string::npos)
        return true;
    return false;
}

//regarder avec Hayden comment est stocker l attribut CGI
bool Response::isCGI(const std::string& path, Location* loc){

}

//Step1: define type of uri (is it a directory or a file) (fonction stat)
//Step1.1: si cest un file, statuscode : 200 et on buildResponse()
//Step2: if it is a dir but doesnt end with / ??? -> error 301 on redirige vers URI/ (on rajoute donc le /)
//Step3: si cest bien un dir, je cherche le fichier index defini dans la config et je tente le path
//Step4: si aucun index trouv;, je regarde si autoindex est on ou off -> off : 403 forbidden
//Step5: si on, genere une page html qui liste le contenu du directory (opendir/readdir)
void Response::handleGetMethod(Request& request){}



void Response::handlePostMethod(Request& request){
//if config->location->allowedmethod.find("POST")
    //uploadfile
//else
    //buildError(403)
}

void Response::handleDeleteMethod(Request& request){}

void Response::buildError(){
    //renvoie la page html en fonction de la map 
    //mais est ce que jutilise cette fonction pour 
}