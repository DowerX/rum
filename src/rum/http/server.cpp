#include <rum/http/server.h>
#include <rum/tcp/error.h>
#include <rum/tcp/server.h>
#include <rum/tcp/utility.h>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

namespace Rum::HTTP {
Server::Server(unsigned int port, size_t worker_count, size_t buffer_size) : Rum::TCP::Server(port), buffer_size(buffer_size), stop(false) {
  for (size_t i = 0; i < worker_count; i++) {
    std::thread worker([this, i]() {
      char* buffer = new char[this->buffer_size]();

      while (true) {
        Task task;

        {
          std::unique_lock<std::mutex> lock(mtx);
          condition.wait(lock, [this]() { return stop || !tasks.empty(); });

          if (stop && tasks.empty()) {
            delete[] buffer;
            return;
          }

          task = tasks.front();
          tasks.pop();
        }

#ifdef DEBUG
        std::cout << "Worker #" << i << " accepted a connection." << std::endl;
#else
        (void)i;
#endif
        handler(task.client_sock, task.sockaddr, buffer);
        if (int status = close(task.client_sock); status == TCP::Error::UNKNOWN) {
          std::cerr << TCP::to_string(task.sockaddr) << ": " << TCP::Error((TCP::Error::Type)status).what() << std::endl;
        }
      }
    });
    workers.emplace_back(std::move(worker));
#ifdef DEBUG
    std::cout << "Worker #" << i << " created" << std::endl;
#endif
  }
}

void Server::end() {
  if (stop)
    return;

  TCP::Server::end();

  {
    std::unique_lock<std::mutex> lock(mtx);
    stop = true;
  }
  condition.notify_all();
  for (auto& worker : workers)
    worker.join();
}

Server::~Server() {
  end();
}

void Server::listen() {
  Rum::TCP::Server::listen([this](int client_sock, sockaddr_in client_address) {
    tasks.emplace(client_sock, client_address);
    condition.notify_one();
  });
}

Method string_to_method(std::string text) {
  if (text.starts_with("GET"))
    return GET;
  if (text.starts_with("HEAD"))
    return HEAD;
  if (text.starts_with("POST"))
    return POST;
  if (text.starts_with("PUT"))
    return PUT;
  if (text.starts_with("DELETE"))
    return DELETE;
  if (text.starts_with("CONNECT"))
    return CONNECT;
  if (text.starts_with("OPTIONS"))
    return OPTIONS;
  // if (text.starts_with("TRACE"))
  return TRACE;
}

void Server::handler(int client_sock, const sockaddr_in& client_address, char* buffer) {
  std::string address = TCP::to_string(client_address);
#ifdef DEBUG
  std::cout << address << ": connected" << std::endl;
#endif
  Request request;

  enum Stage { METHOD, HEADER, BODY };
  Stage stage = METHOD;

  std::string message;

  while (true) {
    ssize_t recieved = recv(client_sock, buffer, buffer_size, 0);
    switch (recieved) {
      case TCP::Error::CLOSED:
        std::cerr << address << ": connection closed" << std::endl;
        return;
      case TCP::Error::UNKNOWN:
        std::cerr << "socket error" << std::endl;
        return;
    }

    message += std::string(buffer, buffer + recieved);

    if (stage == METHOD && message.contains("\r\n")) {
      std::regex method_regex("(GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE) (\\/.*) HTTP.*");
      std::smatch match;
      if (std::regex_match(message.cbegin(), message.cbegin() + message.find("\r\n"), match, method_regex)) {
        try {
          request = Request(string_to_method(match.str(1)), URI("http://0.0.0.0:" + std::to_string(TCP::Server::get_port()) + match.str(2)));
        } catch (std::exception&) {
          return;
        }

        message = message.substr(message.find("\r\n"));
        stage = HEADER;
      } else {
        return;
      }
    }

    if (stage == HEADER && message.contains("\r\n\r\n")) {
      std::regex header_regex("(.*): (.*)");
      for (std::sregex_iterator it = std::sregex_iterator(message.cbegin(), message.cbegin() + message.find("\r\n\r\n"), header_regex);
           it != std::sregex_iterator(); it++) {
        std::string key(it->str(1));
        std::string value(it->str(2));

        if (key == "Host") {
          if (size_t pos = value.find(':'); pos != value.npos) {
            request.get_uri().set_host(value.substr(0, pos));
            request.get_uri().set_port(value.substr(pos + 1));
          } else {
            request.get_uri().set_host(value);
          }
        } else if (key == "Cookie") {
          std::stringstream cookies_string(value);
          std::string cookie;
          while (std::getline(cookies_string, cookie, ';')) {
            if (size_t pos = cookie.find('='); pos != cookie.npos) {
              request.set_cookie(cookie.substr(0, pos), cookie.substr(pos + 1));
            } else {
              return;
            }
          }
        } else {
          std::string key(it->str(1));
          std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
          request.set_header(key, it->str(2));
        }
      }

      message = message.substr(message.find("\r\n\r\n") + 4);

      if (Method method = request.get_method(); method == POST || method == PUT)
        stage = BODY;
      else
        break;
    }

    if (stage == BODY) {
      std::cout << "here" << std::endl;

      try {
        size_t content_length = std::stoul(request.get_header("content-length"));
        std::cout << message.size() << " " << content_length << std::endl;
        if (message.size() < content_length)
          continue;
      } catch (std::out_of_range) {
      } catch (std::invalid_argument e) {
        std::cerr << "invlaid Content-Length header" << std::endl;
      }
      request.set_body(message);
      break;
    }
  }

  try {
    Response resp(client_sock);
    resp.headers["Content-type"] = "text/html";

    bool found = false;

    for (auto it = paths[request.get_method()].cbegin(); it != paths[request.get_method()].cend(); it++) {
      if (std::regex_match(request.get_uri().get_path(), it->first)) {
        it->second(request, resp);
        found = true;
        break;
      }
    }

    if (!found) {
      resp.set_code(404);
      resp.body = "<h1>404: Page not found :C</h1>";
    }
  } catch (std::out_of_range) {
  } catch (TCP::Error) {
  }
}
} // namespace Rum::HTTP