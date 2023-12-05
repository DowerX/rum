#pragma once

#include <netinet/in.h>
#include <string>

namespace Rum::TCP {

std::string to_string(const sockaddr_in& address);

} // namespace Rum::TCP