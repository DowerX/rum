#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <functional>

#define MAX_PENDING 10

namespace Rum::TCP {
class Server {
 private:
  const int sock;
  unsigned short port;
  bool stop;

 public:
  Server(unsigned short port);
  virtual ~Server();

  unsigned short get_port() const { return port; }

  void listen(const std::function<void(int, sockaddr_in)>& handler) const;
  virtual void end() {
    stop = true;
    close(sock);
  }
};
} // namespace Rum::TCP