#include "Client.hpp"

Client() {}

Client::Client(const Client& other) : _clientFd(other._clientFd), _bytesRead(other._bytesRead), _buffer(other._buffer), _bodyStatus(other._bodyStatus) {}

Client::~Client() {}
