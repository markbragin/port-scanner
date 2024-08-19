#include "argparser.hpp"

#include <arpa/inet.h>
#include <exception>
#include <string>

bool isValidIPv4(const std::string &address) {
  struct sockaddr_in sa;
  int res = inet_pton(AF_INET, address.c_str(), &sa);
  return res <= 0 ? false : true;
}

int parsePort(const std::string &str) {
  size_t pos;
  int port;

  try {
    port = std::stoi(str, &pos);
  } catch (const std::exception &e) {
    throw BadPort("Wrong port number\n");
  }

  if (port < 0 || port >= (1 << 16)) {
    throw BadPort("Wrong port number\n");
  }
  return port;
}
