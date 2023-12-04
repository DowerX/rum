#pragma once

#include <string>

namespace Rum::HTTP {
class Response {
 private:
  const int client_sock;
  bool sent_header;
  bool sent_body;
  unsigned int code;

 public:
  Response(int client_sock) : client_sock(client_sock), sent_header(false), sent_body(false), code(200) {}
  ~Response();

  void send_header(const std::string& name, const std::string& value);
  void send_body(const std::string& value);
  void set_code(unsigned int code) { this->code = code; }
};
} // namespace Rum::HTTP