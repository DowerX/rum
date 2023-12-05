#include <rum/http/method.h>
#include <rum/http/request.h>
#include <format>
#include <iostream>
#include <string>

namespace Rum::HTTP {

Request::operator std::string() const {
  std::string headers_string;
  for (auto header : headers) {
    headers_string += "\t\t'" + header.first + "': '" + header.second + "'\n";
  }
  return std::vformat(
      "Request{{\n"
      "\tmethod:  {}\n"
      "\t{}\n"
      "\theaders: \n{}"
      "\tbody:    '{}'\n"
      "}}",
      std::make_format_args(to_string(method), (std::string)uri, headers_string, body));
}

std::ostream& operator<<(std::ostream& stream, const Request req) {
  stream << (std::string)req;
  return stream;
}

} // namespace Rum::HTTP