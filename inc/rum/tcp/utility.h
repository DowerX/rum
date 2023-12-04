#pragma once

#include <netinet/in.h>
#include <string>

namespace Rum::TCP {

std::string address_to_string(sockaddr_in address);

} // namespace Rum::TCP