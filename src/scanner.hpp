#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <arpa/inet.h>
#include <string>
#include <sys/epoll.h>
#include <vector>

void scanTcpPorts(const std::string &address, int from, int to);

static void addToPoll_(int pollingfd, const std::string &address, int port,
                       std::vector<epoll_event> &events);

#endif
