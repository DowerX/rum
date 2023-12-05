#include <rum/http/response.h>
#include <rum/tcp/error.h>
#include <sys/socket.h>
#include <format>
#include <map>
#include <stdexcept>

namespace Rum::HTTP {
std::string to_string(int code) {
  static const std::map<int, std::string> codes{{100, "Continue"},
                                                {101, "Switching Protocols"},
                                                {102, "Processing"},
                                                {103, "Early Hints"},
                                                {200, "OK"},
                                                {201, "Created"},
                                                {202, "Accepted"},
                                                {203, "Non-Authoritative Information"},
                                                {204, "No Content"},
                                                {205, "Reset Content"},
                                                {206, "Partial Content"},
                                                {207, "Multi-Status"},
                                                {208, "Already Reported"},
                                                {226, "IM Used"},
                                                {300, "Multiple Choices"},
                                                {301, "Moved Permanently"},
                                                {302, "Found"},
                                                {303, "See Other"},
                                                {304, "Not Modified"},
                                                {307, "Temporary Redirect"},
                                                {308, "Permanent Redirect"},
                                                {400, "Bad Request"},
                                                {401, "Unauthorized"},
                                                {402, "Payment Required"},
                                                {403, "Forbidden"},
                                                {404, "Not Found"},
                                                {405, "Method Not Allowed"},
                                                {406, "Not Acceptable"},
                                                {407, "Proxy Authentication Required"},
                                                {408, "Request Timeout"},
                                                {409, "Conflict"},
                                                {410, "Gone"},
                                                {411, "Length Required"},
                                                {412, "Precondition Failed"},
                                                {413, "Content Too Large"},
                                                {414, "URI Too Long"},
                                                {415, "Unsupported Media Type"},
                                                {416, "Range Not Satisfiable"},
                                                {417, "Expectation Failed"},
                                                {418, "I'm a teapot"},
                                                {421, "Misdirected Request"},
                                                {422, "Unprocessable Content"},
                                                {423, "Locked"},
                                                {424, "Failed Dependency"},
                                                {425, "Too Early"},
                                                {426, "Upgrade Required"},
                                                {428, "Precondition Required"},
                                                {429, "Too Many Requests"},
                                                {431, "Request Header Fields Too Large"},
                                                {451, "Unavailable For Legal Reasons"},
                                                {500, "Internal Server Error"},
                                                {501, "Not Implemented"},
                                                {502, "Bad Gateway"},
                                                {503, "Service Unavailable"},
                                                {504, "Gateway Timeout"},
                                                {505, "HTTP Version Not Supported"},
                                                {506, "Variant Also Negotiates"},
                                                {507, "Insufficient Storage"},
                                                {508, "Loop Detected"},
                                                {510, "Not Extended"},
                                                {511, "Network Authentication Required"}};

  try {
    return codes.at(code);
  } catch (std::out_of_range) {
    return codes.at((code / 100) * 100);
  }
}

Response::~Response() {
  std::string headers_string;
  for (auto header : headers) {
    headers_string += header.first + ": " + header.second + "\r\n";
  }
  for (auto cookie : cookies) {
    headers_string += "Set-cookie: " + cookie.first + "=" + cookie.second + "\r\n";
  }

  std::string message = std::vformat(
      "HTTP/1.1 {} {}\r\n"
      "{}"
      "\r\n"
      "{}",
      std::make_format_args(std::to_string(code), to_string(code), headers_string, body));
  send(client_sock, message.c_str(), message.size(), 0);
}
} // namespace Rum::HTTP