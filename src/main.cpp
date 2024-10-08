#include <iostream>

#include "argparser.hpp"
#include "scanner.hpp"

void printUsage() {
  std::cout << "USAGE: scanner <IP> <START_PORT> <END_PORT>\n"
            << "  IP - the address of a machine\n"
            << "  START_PORT - the first port to scan\n"
            << "  END_PORT - the last port to scan\n";
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printUsage();
    return -1;
  }

  if (!isValidIPv4(argv[1])) {
    std::cerr << "[ERROR] Invalid IPv4 address\n";
    return -2;
  }

  int fromPort, toPort;
  try {
    fromPort = parsePort(argv[2]);
    toPort = parsePort(argv[3]);
  } catch (const BadPort &e) {
    std::cerr << "[ERROR] Port number must be in a range of [0..65535]\n";
    return -3;
  }

  if (fromPort > toPort) {
    std::swap(fromPort, toPort);
  }

  scanTcpPorts(argv[1], fromPort, toPort);

  return 0;
}
