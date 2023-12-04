#include <rum/http/method.h>

namespace Rum::HTTP {
std::string method_to_string(Method m) {
  switch (m) {
    case GET:
      return "GET";
    case HEAD:
      return "HEAD";
    case POST:
      return "POST";
    case PUT:
      return "PUT";
    case DELETE:
      return "DELETE";
    case CONNECT:
      return "CONNECT";
    case OPTIONS:
      return "OPTIONS";
    case TRACE:
      return "TRACE";
  }
}
} // namespace Rum::HTTP
