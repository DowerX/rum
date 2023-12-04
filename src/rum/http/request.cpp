#include <rum/http/request.h>
#include <iostream>
#include <string>

namespace Rum::HTTP {

Request::operator std::string() const {
  std::string result("Request{\n\tmethod: " + method_to_string(method) + "\n\tpath: \"" + path + "\"\n\theaders:\n");
  for (auto header : get_headers()) {
    result += "\t\t\"" + header.first + "\": \"" + header.second + "\"\n";
  }

  result += "\tbody: \"" + body + "\"\n}";

  return result;
}

std::ostream& operator<<(std::ostream& stream, const Request req) {
  stream << (std::string)req;
  return stream;
}

} // namespace Rum::HTTP