#pragma once

#include <netinet/in.h>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <regex>
#include <vector>
#include <rum/tcp/server.h>
#include <rum/http/request.h>
#include <rum/http/response.h>

#define DEFAULT_WORKER_COUNT 10
#define DEFAULT_BUFFER_SIZE  8196

namespace Rum::HTTP {

class Server : public Rum::TCP::Server {
 private:
  struct Task {
    Task() {}
    Task(int client_sock, const sockaddr_in& sockaddr) : client_sock(client_sock), sockaddr(sockaddr) {}
    int client_sock;
    sockaddr_in sockaddr;
  };

  const size_t buffer_size;

  std::vector<std::thread> workers;
  std::queue<Task> tasks;
  std::mutex mtx;
  std::condition_variable condition;
  bool stop;

  std::map<Method, std::vector<std::pair<std::regex, std::function<void(const Request&, Response&)>>>> paths;

  void handler(int client_sock, const sockaddr_in& client_address, char* buffer);

 public:
  Server(unsigned int port, size_t worker_count, size_t buffer_size);
  Server(unsigned int port) : Server(port, DEFAULT_WORKER_COUNT, DEFAULT_BUFFER_SIZE) {}
  ~Server();
  void listen();

  void end() override;

  template <Method M>
  void add_path(const std::string& path, const std::function<void(const Request&, Response&)>& callback) {
    paths[M].push_back(std::pair<std::regex, std::function<void(const Request&, Response&)>>(std::regex(path), callback));
  }
};

} // namespace Rum::HTTP