#include "Client.hpp"

Client::Client() : _clientFd(-1), _bytesRead(0), _buffer(0), _bodyStatus(-1)
{}

Client::Client(const Client& other) : _clientFd(other._clientFd), _bytesRead(other._bytesRead), _buffer(other._buffer), _bodyStatus(other._bodyStatus) {}

Client::~Client() {}
