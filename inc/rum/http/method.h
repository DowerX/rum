#pragma once

#include <string>
namespace Rum::HTTP {

enum Method { GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE };

std::string to_string(Method method);
} // namespace Rum::HTTP