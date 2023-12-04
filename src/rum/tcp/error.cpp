#include <rum/tcp/error.h>

namespace Rum::TCP {

const char* Error::what() const noexcept {
  switch (type) {
    case CLOSED:
      return "the connection is already closed";
    case UNKNOWN:
      return "an unknown error has occured";
  }
}
} // namespace Rum::TCP