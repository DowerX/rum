#pragma once

#include <rum/http/method.h>
#include <rum/http/uri.h>
#include <map>
#include <string>

namespace Rum::HTTP {
class Request {
 public:
  Request() : method(GET), uri(URI()), body("") {}
  Request(Method method, const URI& uri) : method(method), uri(uri), body("") {}

  Method get_method() const { return method; }
  URI& get_uri() { return uri; }

  const std::map<std::string, std::string>& get_headers() const { return headers; }
  std::string get_header(std::string name) const { return headers.at(name); }
  void set_header(std::string name, std::string value) { headers[name] = value; }

  const std::map<std::string, std::string>& get_cookies() const { return cookies; }
  std::string get_cookie(std::string name) const { return cookies.at(name); }
  void set_cookie(std::string name, std::string value) { cookies[name] = value; }

  std::string get_body() const { return body; }
  void set_body(std::string body) { this->body = body; }

  operator std::string() const;

  friend std::ostream& operator<<(std::ostream& stream, const Request req);

 private:
  Method method;
  URI uri;
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> cookies;
  std::string body;
};
} // namespace Rum::HTTP