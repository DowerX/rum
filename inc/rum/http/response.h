#pragma once

#include <rum/http/cookie.h>
#include <map>
#include <string>
#include <vector>

namespace Rum::HTTP {
class Response {
 private:
  const int client_sock;
  unsigned int code;

 public:
  Response(int client_sock) : client_sock(client_sock), code(200) {}
  ~Response();

  void set_code(unsigned int code) {
    if (code >= 100 && code < 600)
      this->code = code;
  }

  std::vector<Cookie> cookies;
  std::map<std::string, std::string> headers;
  std::string body;
};
} // namespace Rum::HTTP