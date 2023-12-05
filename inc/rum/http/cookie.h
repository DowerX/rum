#pragma once

#include <string>

namespace Rum::HTTP {

class Cookie {
 private:
  std::string cookie;

 public:
  Cookie(const std::string& name, const std::string& value, const std::string& path, unsigned int lifetime);

  operator std::string() const;
};

} // namespace Rum::HTTP