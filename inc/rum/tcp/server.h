#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <functional>

namespace Rum::TCP {
class Server {
 private:
  const int sock;
  bool stop;

 public:
  Server(unsigned short port);
  virtual ~Server();
  void listen(std::function<void(int, sockaddr_in)> handler) const;
  virtual void end() {
    stop = true;
    close(sock);
  }
};
} // namespace Rum::TCP