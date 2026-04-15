#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>


//CE QU'IL TE RESTE A FAIRE :
//-finir POST METHOD
//-faire DELETE METHOD
//- build error (regarder les rfc) - on est daccord que cest la qu'on ajoute les pages html ?
//- ajouter les reasons phrases
//- creer la fonction de responses formats
//- mettre fonction findLocation dans ServerConfig

//-----------------------

//Step1: verifier que le request status code soit != 0, if yes buildError(statusCode) -- OK
//Step2 : findlocation verifier quil existe ? if not utilise le root du server -- OK
//Step3: y a t il une redirection ? if yes stop et buildError(300); -- OK
//Step4: la methode est elle authorise ? if no stop buildError(405) -- OK;
//Step5: est ce que c'est un CGI ? if yes execute CGI et return; -- OK
//Step6: gerer les requests en fonction de la methode -- EN COURS
void Response::build(Request& request, ServerConfig *config, char **envp){
    if (request.getStatusCode() != 0){
        _statusCode = request.getStatusCode();
        buildError();
        return ;
    }
    Location* loc = findLocation(request.getPath(), config); //si on met dans serverConfig config->findLocation
    _internalPath = buildFileSystemPath(loc, request.getPath());
    struct stat sb;
    //prendre la fonction de Josephine = extractserverpath(internalPath) et remplacer dans stat et isCGI
    //voir dans quel ordre faire ca
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError();
        return;
    }
    _isDir = S_ISDIR(sb.st_mode);
    if (loc->getRedirectCode() != 0){
        _statusCode = loc->getRedirectCode();
        _headers["Location"] = loc->getNewPath();
        return ;
    }
    if (!methodAllowed(loc, request.getMethod())){
        _statusCode = METHOD_NOT_ALLOWED;
        buildError();
        return ;
    }
    if (isCGI(_internalPath, loc)){
        handleCGI(request, envp);
        return;
    }
    handleMethod(request, loc);
}

void Response::handleMethod(Request& request, Location* loc){
    if (request.getMethod() == "GET")
        handleGetMethod(request, loc);
    else if (request.getMethod() == "POST")
        handlePostMethod(request, loc);
    else
        handleDeleteMethod(request, loc);
}

//Step1: trouver si le URI correspond a une des locations -- OK
//Step2: mettre cette fonction dans ServerConfig 
//Step3: renvoyer le nom de la location qui correpond le plus a la URI
//Step4: est ce quil faut que jaggrege le root path ici 
Location* Response::findLocation(const std::string& url, ServerConfig* config){
    Location *best = NULL;
    size_t bestLen = 0;

    std::vector<Location>& locs = config->getLocations();
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

std::string Response::buildFileSystemPath(Location* loc, const std::string& url){    
    std::string remain = url.substr(loc->getPath().size());
    return loc->getRoot() + "/" + remain; 
}

bool Response::methodAllowed(Location *loc, const std::string& method){
    std::string methodAllowed = loc->getMethodAllowed();
    if (methodAllowed.find(method) != std::string::npos)
        return true;
    return false;
}

bool Response::isCGI(const std::string& path, Location* loc){
    if (loc->getCgiEnabled()){
        std::vector<std::string>::iterator it = loc->getCgiExtensions().begin();
        
        for (; it != loc->getCgiExtensions().end(); it++){
            size_t pos = path.find(*it) + (*it).size();
            if (pos != std::string::npos && (path[pos] == *path.end() || path[pos] == '/'))
                return true;
        }
        return false;
    }
    return false;
}

void Response::setSyscallError(){
    if (errno == ENOENT || errno == ENOTDIR)
        _statusCode = NOT_FOUND;
    else if (errno == EACCES)
        _statusCode = FORBIDDEN;
    else
        _statusCode = INTERNAL_SERVER_ERROR;
    buildError();
}

//Step1: define type of uri (is it a directory or a file) (fonction stat) -- OK
//Step1.1: si cest un file, statuscode : 200 et on buildResponse() -- OK
//Step2: if it is a dir but doesnt end with / ??? -> error 301 on redirige vers URI/ (on rajoute donc le /) -- OK
//Step3: si cest bien un dir, je cherche le fichier index defini dans la config et je tente le path -- OK
//Step4: si aucun index trouv;, je regarde si autoindex est on ou off -> off : 403 forbidden / -- OK
//Step5: si oui on, genere une page html qui liste le contenu du directory (opendir/readdir) -- OK
void Response::handleGetMethod(Request& request, Location* loc){
    if (!_isDir){
        std::ifstream file(_internalPath.c_str());
        if (!file.is_open())
            return (setSyscallError());
        std::ostringstream ss;
        ss << file.rdbuf();
        _body = ss.str();
        _statusCode = OK;
        return ;
    }
    else if (_isDir){
        if (request.getPath()[request.getPath().size() - 1] != '/'){
            _statusCode = MOVED_PERMANENTLY;
            _headers["Location"] = request.getPath() + "/";
            return ;
        }
        std::vector<std::string> indexes = loc->getIndexes();
        std::vector<std::string>::iterator it = indexes.begin();
        for (; it != indexes.end(); it++){
            std::ifstream file(*it);
            if (file.is_open()){
                _statusCode = OK;
                return ;
            }
        }
        if (!loc->getAutoIndex())
            return (setSyscallError());
        displayAutoIndex();
    }
    else {
        _statusCode = FORBIDDEN;
        return (buildError());
    }
}

void Response::displayAutoIndex(){
    DIR *dir = opendir(_internalPath.c_str());
    if (dir == NULL){
        setSyscallError();
        return ;
    }
    std::string html = "<html><body><h1>Index of " + _internalPath + "</h1><ul>";
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL ){
        std::string name = entry->d_name;
        if (name == ".") continue;
        html += "<li><a href=\"" + name + "\">" + name + "</a></li>";
    }
    closedir(dir);
    html += "</ul></body></html>";
    _body = html;
    _statusCode = OK;
}

//Step 1: define if it is a DIR or a FILE
//Step 2: DIR : verify if i can open it and create a file containing the body of the request
//Step 3: FILE : add the body to the file
//Step 4: return ;

//dire a Hayden que jai besoin de bool _upload_alllowed et le lieu de lupload
void Response::handlePostMethod(Request& request, Location* loc){
    if (!loc->getUploadAllowed()){
        _statusCode = FORBIDDEN;
        buildError();
        return ;
    }
    if (_isDir){
        DIR *dir = opendir(_internalPath.c_str());
        if (dir == NULL){
            setSyscallError();
            return ;
        }

    }
    
}

void Response::handleDeleteMethod(Request& request, Location* loc){}

void Response::buildError(){
    //renvoie la page html en fonction de la map 
    //mais est ce que jutilise cette fonction pour 
}