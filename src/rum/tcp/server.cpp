#include <arpa/inet.h>
#include <rum/tcp/error.h>
#include <rum/tcp/server.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <functional>
#include <string>

namespace Rum::TCP {

std::string to_string(const sockaddr_in& address) {
  return std::string(inet_ntoa(address.sin_addr)) + ":" + std::to_string(ntohs(address.sin_port));
}

Server::Server(unsigned short port) : sock(socket(AF_INET, SOCK_STREAM, 0)), port(port), stop(false) {
  if (sock == -1)
    throw Error(Error::UNKNOWN);

  std::signal(SIGPIPE, SIG_IGN);

  sockaddr_in address = {.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {.s_addr = INADDR_ANY}};

  if (bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1)
    throw Error(Error::UNKNOWN);
}

Server::~Server() {
  end();
}

void Server::listen(const std::function<void(int, sockaddr_in)>& handler) const {
  if (::listen(sock, MAX_PENDING) == -1)
    throw Error(Error::UNKNOWN);

  while (!stop) {
    sockaddr_in client_address;
    socklen_t client_address_len(sizeof(client_address));
    int client_sock = accept(sock, (struct sockaddr*)&client_address, &client_address_len);
    if (client_sock == -1)
      continue;

    handler(client_sock, client_address);
  }
}

} // namespace Rum::TCP