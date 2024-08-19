#ifndef ARG_PARSER_HPP_
#define ARG_PARSER_HPP_

#include <exception>
#include <string>

bool isValidIPv4(const std::string &address);
int parsePort(const std::string &str);

class BadPort : public std::exception {
  std::string whatMsg;

public:
  BadPort(const char *msg) : whatMsg(msg) {}
  const char *what() const noexcept override { return whatMsg.c_str(); }
};

#endif
