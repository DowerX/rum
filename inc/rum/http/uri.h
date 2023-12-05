#pragma once

#include <format>
#include <map>
#include <string>

namespace Rum::HTTP {
class URI {
 private:
  std::string scheme;
  std::string user;
  std::string password;
  std::string host;
  std::string port;
  std::string path;
  std::map<std::string, std::string> parameters;
  std::string fragment;

 public:
  URI(const std::string& uri);
  URI() {}

  const std::string& get_scheme() const { return scheme; }
  const std::string& get_user() const { return user; }
  const std::string& get_password() const { return password; }
  const std::string& get_host() const { return host; }
  const std::string& get_port() const { return port; }
  const std::string& get_path() const { return path; }
  const std::map<std::string, std::string>& get_parameters() const { return parameters; }
  const std::string& get_fragment() const { return fragment; }

  operator std::string() const;
};
} // namespace Rum::HTTP