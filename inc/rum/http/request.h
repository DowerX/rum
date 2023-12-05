#pragma once

#include <map>
#include <string>
#include "method.h"
#include "rum/http/uri.h"

namespace Rum::HTTP {
class Request {
 public:
  Request() : method(GET), uri(URI()), body("") {}
  Request(Method method, const URI& uri) : method(method), uri(uri), body("") {}

  Method get_method() const { return method; }
  const URI& get_uri() const { return uri; }

  const std::map<std::string, std::string>& get_headers() const { return headers; }
  std::string get_header(std::string name) const { return headers.at(name); }
  void set_header(std::string name, std::string value) { headers[name] = value; }

  std::string get_body() const { return body; }
  void set_body(std::string body) { this->body = body; }

  operator std::string() const;

  friend std::ostream& operator<<(std::ostream& stream, const Request req);

 private:
  Method method;
  URI uri;
  std::map<std::string, std::string> headers;
  std::string body;
};
} // namespace Rum::HTTP