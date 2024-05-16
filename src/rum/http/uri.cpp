#include <rum/http/uri.h>
#include <exception>
#include <iostream>
#include <regex>
#include <format>
#include <string>

namespace Rum::HTTP {

URI::operator std::string() const {
  std::string query;
  for (auto param : parameters) {
    query += "\t\t'" + param.first + "': '" + param.second + "'\n";
  }

  return std::vformat(
      "URI{{"
      "\n\tscheme:   '{}'"
      "\n\tuser:     '{}'"
      "\n\tpassword: '{}'"
      "\n\thost:     '{}'"
      "\n\tport:     '{}'"
      "\n\tpath:     '{}'"
      "\n\tquery:   \n{}"
      "\n\tfragment: '{}'"
      "\n}}",
      std::make_format_args(scheme, user, password, host, port, path, query, fragment));
}

URI::URI(const std::string& uri) {
  std::regex uri_regex(R"((([\w\d]+):\/\/)?(([\w\d]+)(:([\w\d]+)?)@)?([\w\d\.]+)(:(\d+))?(\/?[\w\d\.\/]+)?(\?([^#]+))?(\#([\w\d]*))?)");

  std::smatch match;

#ifdef DEBUG
  std::cout << uri << std::endl;
#endif

  if (std::regex_match(uri.cbegin(), uri.cend(), match, uri_regex)) {
#define MATCH(var, i)   \
  if (match[i].matched) \
    var = match.str(i);

    MATCH(scheme, 2)
    MATCH(user, 4)
    MATCH(password, 6)
    MATCH(host, 7)
    MATCH(port, 9)
    MATCH(path, 10)
    std::string query;
    MATCH(query, 12)
    MATCH(fragment, 14)

    std::stringstream parameters_string(query);
    std::string parameter;
    while (std::getline(parameters_string, parameter, '&')) {
      if (size_t pos = parameter.find('='); pos != parameter.npos) {
        parameters[parameter.substr(0, pos)] = parameter.substr(pos + 1);
      } else {
        parameters[parameter] = "";
      }
    }
  } else {
    throw std::exception();
  }
}
} // namespace Rum::HTTP