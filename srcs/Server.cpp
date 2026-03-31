#include "Server.hpp"

Server::Server() : _listenFd(-1), _port(0), _root(""), _clients()  
{}

Server::Server(const Server& other) : _listenFd(other._listenFd), _port(other._port), _root(other._root), _clients(other._clients) {}

Server::~Server() {}
