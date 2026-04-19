#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"
#include "../include/Debug.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>


//CE QU'IL TE RESTE A FAIRE :
//-finir POST METHOD -- OK
//-faire DELETE METHOD -- OK
//- build error (regarder les rfc) - on est daccord que cest la qu'on ajoute les pages html ? -- OK
//- ajouter les reasons phrases -- OK
//- creer la fonction de responses formats -- OK
//- mettre fonction findLocation dans ServerConfig

//-----------------------

//Step1: verifier que le request status code soit != 0, if yes buildError(statusCode) -- OK
//Step2 : findlocation verifier quil existe ? if not utilise le root du server -- OK
//Step3: y a t il une redirection ? if yes stop et buildError(300); -- OK
//Step4: la methode est elle authorise ? if no stop buildError(405) -- OK;
//Step5: est ce que c'est un CGI ? if yes execute CGI et return; -- OK
//Step6: gerer les requests en fonction de la methode -- OK
void Response::build(Request& request, ServerConfig *config, char **envp){
    _loc = findLocation(request.getPath());
    _config = config;
    if (request.getStatusCode() != 0){
        _statusCode = request.getStatusCode();
        buildError();
        return ;
    }
     //si on met dans serverConfig config->findLocation
    _internalPath = buildFileSystemPath(request.getPath());
    
    if (_loc->getRedirectCode() != 0){
        _statusCode = _loc->getRedirectCode();
        _headers["Location"] = _loc->getNewPath();
        return ;
    }
    if (!methodAllowed(request.getMethod())){
        _statusCode = METHOD_NOT_ALLOWED;
        buildError();
        return ;
    }
    if (isCGI()){
        handleCGI(request, envp);
        return;
    }
    handleMethod(request);
}

void Response::handleMethod(Request& request){
    if (request.getMethod() == "GET")
        handleGetMethod(request);
    else if (request.getMethod() == "POST")
        handlePostMethod(request);
    else
        handleDeleteMethod(request);
}

//Step1: trouver si le URI correspond a une des locations -- OK
//Step2: mettre cette fonction dans ServerConfig -- OK
//Step3: renvoyer le nom de la location qui correpond le plus a la URI -- OK
//Step4: est ce quil faut que jaggrege le root path ici -- OK
Location* Response::findLocation(const std::string& url){
    Location *best = NULL;
    size_t bestLen = 0;

    std::vector<Location>& locs = _config->getLocations();
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

std::string Response::buildFileSystemPath(const std::string& url){    
    _remain = url.substr(_loc->getPath().size());
    return _loc->getRoot() + "/" + _remain; 
}

bool Response::methodAllowed(const std::string& method){
    std::string methodAllowed = _loc->getMethodAllowed();
    if (methodAllowed.find(method) != std::string::npos)
        return true;
    return false;
}

bool Response::isCGI(const std::string& path){
    if (_loc->getCgiEnabled()){
        std::vector<std::string>::iterator it = _loc->getCgiExtensions().begin();
        
        for (; it != _loc->getCgiExtensions().end(); it++){
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
void Response::handleGetMethod(Request& request){
    struct stat sb;
   
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError();
        return;
    }
    if (!S_ISDIR(sb.st_mode)){
        std::ifstream file(_internalPath.c_str());
        if (!file.is_open())
            return (setSyscallError());
        std::ostringstream ss;
        ss << file.rdbuf();
        _body = ss.str();
        _statusCode = OK;
        _headers["Content-Type"] = extractContentType();
        return ;
    }
    else if (S_ISDIR(sb.st_mode)){
        if (request.getPath()[request.getPath().size() - 1] != '/'){
            _statusCode = MOVED_PERMANENTLY;
            _headers["Location"] = request.getPath() + "/";
            return ;
        }
        std::vector<std::string> indexes = _loc->getIndexes();
        std::vector<std::string>::iterator it = indexes.begin();
        for (; it != indexes.end(); it++){
            std::ifstream file(*it);
            if (file.is_open()){
                _statusCode = OK;
                return ;
            }
        }
        if (!_loc->getAutoIndex()){
            setSyscallError();
            return ;
        }
        displayAutoIndex();
    }
    else {
        _statusCode = FORBIDDEN;
        buildError();
        return ;
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
    _headers["Content-Type"] = "text/html";
}


// A DISCUTER :
// - directive upload_store ou upload ? -- OK
// - upload store obligatoire ?  -- OUI -- OK
// - si l url est un dossier qui se terminer par / -> ERREUR -- OK
// - si url contient des sous dossier non existant -> TEST STAT UPLOADDIR -- OK
// - si cest un fichier sans extension on le cree tel quel ? -> COMME LINUX CREER TELQUEL
// - si le fichier existe deja, on refuse --> ERREUR REFUSE : conflit 409 -- OK
// - est ce qu on gere le multipart ? --> OUI MAIS UNIQUEMENT PAR CGI
// car si on uploads depuis un vrai navigateur on est oblige mais via curl et telnet cest okay sans


//Step 1: check if upload allowed in location -- OK
//Step 2: define if it is a DIR or a FILE -- OK
//Step 3: DIR create a file containing the body of the request, generate name of the file according to content-disposition if existing
//Step 4: EXISTING FILE : add the body to the file or replace it -- OK
//Step 5: NON-EXISTING FILE : create file and body to the file

void Response::handlePostMethod(Request& request){
    if (request.getHeaders("content-type").find("multipart/form-data") != std::string::npos){
        _statusCode = FORBIDDEN;
        buildError();
        LOG(std::cout << "can't handle multipart without cgi" << std::endl);
        return ;
    }
    if (!_loc->getUploadStore().empty()){
        _statusCode = FORBIDDEN;
        buildError();
        LOG(std::cout << "can't upload, no upload store" << std::endl);
        return ;
    }
    std::string uploadDir = _loc->getUploadStore();
    struct stat dirStat;
    if (stat(uploadDir.c_str(), &dirStat) != 0 || !S_ISDIR(dirStat.st_mode)){
        setSyscallError();
        LOG(std::cout << "upload folder doesnt exist" << std::endl);
        return ;
    }
    std::string uploadPath = _loc->getUploadStore() + "/" + _remain;
    struct stat fileStat;
    if (stat(uploadPath.c_str(), &fileStat) == 0){
        _statusCode = CONFLICT;
        buildError();
        LOG(std::cout << "file already exist" << std::endl);
        return ;
    }
    if (S_ISDIR(fileStat.st_mode)){
        _statusCode = FORBIDDEN;
        LOG(std::cout << "can't upload body without specifying the file in URL" << std::endl);
        return ;
    }
    else {
        std::string fileName = extractFileName(request.getPath());
        std::ofstream file(fileName);
        file << request.getBody();
        file.close();
        _body = request.getBody();
        _statusCode = CREATED;
        _headers["Content-Type"] = request.getHeaders("content-type");
    }
}

std::string extractFileName(std::string path){
    std::string fileName;
    return (fileName);
}


//Step1 : define if file ou directory is available
//Step2: if directory but ambiguous url that doesnt end wiht / -> 409
//Step3: try to remove if no -> syscall if yes -> 204 : no content action succesful 

void Response::handleDeleteMethod(Request& request){
    struct stat sb;
   
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError();
        return;
    }
    if (S_ISDIR(sb.st_mode)){
        if (request.getPath()[request.getPath().size() - 1] != '/'){
            _statusCode = CONFLICT;
            return ;
        }
    }
    if (std::remove(_internalPath.c_str()) != 0){
        setSyscallError();
        return;
    }
    _statusCode = NO_CONTENT;
}


void Response::buildError(){

    _reasonPhrase = getReasonPhrase(_statusCode);
    std::string errorPageFile;
    if (_loc && !_loc->getErrorPage(_statusCode).empty())
        errorPageFile = _loc->getRoot() + "/" + _loc->getErrorPage(_statusCode);
    else if (!_config->getErrorPage(_statusCode).empty())
        errorPageFile = _config->getRoot() + "/" + _config->getErrorPage(_statusCode);
    if (!errorPageFile.empty()){
        struct stat errorPath;
        if (stat(errorPageFile.c_str(), &errorPath) == 0 && S_ISREG(errorPath.st_mode)){
            std::ifstream file(errorPageFile.c_str());
            if (file.is_open()){
                std::ostringstream ss;
                ss << file.rdbuf();
                _body = ss.str();
                _headers["Content-Type"] = extractContentType();
                return ;
            }
        }
    }
    generateErrorPage();
}

void Response::generateErrorPage()
{
    std::ostringstream body;

    body << "<html>\n<head><title>" << _statusCode << " " << _reasonPhrase << "</title></head>\n"
         << "<body>\n"
         << "<h1>" << _statusCode << " " << _reasonPhrase << "</h1>\n"
         << "<hr>\n"
         << "<p>webserv</p>\n"
         << "</body>\n</html>\n";

    _body = body.str();
    _headers["Content-Type"] = "text/html";
}

