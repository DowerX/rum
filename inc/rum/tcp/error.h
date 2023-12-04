#pragma once

#include <exception>

namespace Rum::TCP {

class Error : public std::exception {
 public:
  enum Type { CLOSED = 0, UNKNOWN = -1 };

  Error(Type type) : type(type) {}

  const char* what() const noexcept override;

 private:
  Type type;
};
} // namespace Rum::TCP