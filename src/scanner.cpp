#include "scanner.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static uint64_t packPortAndFd_(uint32_t port, uint32_t fd) {
  return (static_cast<uint64_t>(port) << 32) + fd;
}

static uint32_t getPort_(uint64_t data) { return data >> 32; }

static uint32_t getFd_(uint64_t data) { return data; }

static void addToPoll_(int pollingfd, const std::string &address, int port,
                       std::vector<epoll_event> &events);

/* Speed of scanning depends on the limit of file descriptors the process
 * can open. On linux default 1024. Increating the limit to (1 << 16)
 * must speed up the scanning process. */
void scanTcpPorts(const std::string &address, int fromPort, int toPort) {
  const int openFdLimit = getdtablesize();
  // subtract 4 since stdin, stdout, stderr, epoll already opened
  const int queueSize = std::min(openFdLimit - 4, toPort - fromPort + 1);
  const int timeout = 300; // ms

  int pollingfd = epoll_create(1);

  if (pollingfd == -1) {
    std::cerr << "epoll_create():" << strerror(errno) << '\n';
    return;
  }

  // it's possible to reduce memory usage here (size = toPort - fromPort + 1)
  // but idx = port is convenient
  std::vector<epoll_event> events(toPort + 1);

  int port;
  for (port = fromPort; port < fromPort + queueSize; port++) {
    addToPoll_(pollingfd, address, port, events);
  }

  std::cout << "Scanning...\n";

  auto pevents = std::make_unique<epoll_event[]>(queueSize);
  int readyCnt = 1;
  int openPortCounter = 0;

  while (readyCnt > 0) {
    readyCnt = epoll_wait(pollingfd, pevents.get(), queueSize, timeout);
    if (readyCnt == -1) {
      std::cerr << "epoll_wait(): " << strerror(errno) << '\n';
    } else {
      for (int i = 0; i < readyCnt; i++) {
        int curPort = getPort_(pevents[i].data.u64);
        int curSock = getFd_(pevents[i].data.u64);
        close(curSock);

        if (port <= toPort) {
          addToPoll_(pollingfd, address, port++, events);
        }

        if (!(pevents[i].events & EPOLLERR)) {
          std::cout << std::setw(6) << std::left << curPort << " opened\n";
          openPortCounter++;
        }
      }
    }
  }

  close(pollingfd);
  std::cout << '[' << openPortCounter << " TCP ports are opened]\n";
}

static void addToPoll_(int pollingfd, const std::string &address, int port,
                       std::vector<epoll_event> &events) {
  sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  inet_pton(AF_INET, address.c_str(), &sa.sin_addr);

  int sockfd;
  if ((sockfd = socket(sa.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
    std::cerr << "socket(): " << strerror(errno) << '\n';
    return;
  }

  if (connect(sockfd, (sockaddr *)&sa, sizeof(sa)) == -1 &&
      errno != EINPROGRESS) {
    close(sockfd);
    std::cerr << "connect(): " << strerror(errno) << '\n';
    return;
  }

  auto &ev = events[port];
  ev.events = EPOLLOUT | EPOLLERR | EPOLLONESHOT;
  ev.data.u64 = packPortAndFd_(port, sockfd);

  if (epoll_ctl(pollingfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
    std::cerr << "epoll_ctl(): " << strerror(errno) << '\n';
    close(sockfd);
  }
}
