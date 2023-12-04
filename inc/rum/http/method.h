#pragma once

#include <string>
namespace Rum::HTTP {

enum Method { GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE };

std::string method_to_string(Method m);
} // namespace Rum::HTTP