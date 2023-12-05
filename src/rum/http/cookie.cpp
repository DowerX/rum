#include <rum/http/cookie.h>
#include <sys/select.h>
#include <chrono>
#include <ctime>

namespace Rum::HTTP {

Cookie::Cookie(const std::string& name, const std::string& value, const std::string& path, unsigned int lifetime)
    : cookie(name + "=" + value + "; path=" + path + "; expires=") {
  auto currentTime = std::chrono::system_clock::now();
  auto newTime = currentTime + std::chrono::seconds(lifetime);
  std::time_t time = std::chrono::system_clock::to_time_t(newTime);

  std::tm* tmInfo = std::gmtime(&time);
  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tmInfo);
  cookie += std::string(buffer);
}

Cookie::operator std::string() const {
  return cookie;
}
}; // namespace Rum::HTTP
