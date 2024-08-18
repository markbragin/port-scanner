#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <arpa/inet.h>
#include <string>
#include <sys/epoll.h>

void scanTcpPorts(const std::string &address, int from, int to);

#endif
