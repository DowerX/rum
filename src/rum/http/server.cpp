#include <rum/http/server.h>
#include <rum/tcp/error.h>
#include <rum/tcp/utility.h>
#include <iostream>
#include <regex>

namespace Rum::HTTP {
Server::Server(unsigned int port, size_t worker_count) : Rum::TCP::Server(port), stop(false) {
  for (size_t i = 0; i < worker_count; i++) {
    std::thread worker([this, i]() {
      char* buffer = new char[BUFFER_LEN]();

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

        std::cout << "Worker #" << i << " accepted a connection." << std::endl;

        handler(task.client_sock, task.sockaddr, buffer);
        if (int status = close(task.client_sock); status == TCP::Error::UNKNOWN) {
          std::cerr << TCP::address_to_string(task.sockaddr) << ": " << TCP::Error((TCP::Error::Type)status).what() << std::endl;
        }
      }
    });
    workers.emplace_back(std::move(worker));
    std::cout << "Worker #" << i << " created" << std::endl;
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
  std::string address = TCP::address_to_string(client_address);
  std::cout << address << ": connected" << std::endl;

  Request request;

  enum Stage { METHOD, HEADER, BODY };
  Stage stage = METHOD;

  std::string message;

  while (true) {
    ssize_t recieved = recv(client_sock, buffer, BUFFER_LEN, 0);
    switch (recieved) {
      case TCP::Error::CLOSED:
        std::cout << address << ": connection closed" << std::endl;
        return;
      case TCP::Error::UNKNOWN:
        std::cout << "socket error" << std::endl;
        return;
    }

    message += std::string(buffer, buffer + recieved);

    if (stage == METHOD && message.contains("\r\n")) {
      std::regex method_regex("(GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE) (\\/.*) .*");
      std::smatch match;
      if (std::regex_match(message.cbegin(), message.cbegin() + message.find("\r\n"), match, method_regex)) {
        request = Request(string_to_method(match.str(1)), match.str(2));
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
        request.set_header(it->str(1), it->str(2));
      }
      message = message.substr(message.find("\r\n\r\n"));

      if (Method method = request.get_method(); method == POST || method == PUT)
        stage = BODY;
      else
        break;
    }

    if (stage == BODY) {
      request.set_body(message);
      break;
    }
  }

  std::cout << request << std::endl;

  try {
    Response resp(client_sock);

    bool found = false;

    for (auto it = paths[request.get_method()].cbegin(); it != paths[request.get_method()].cend(); it++) {
      if (std::regex_match(request.get_path(), it->first)) {
        it->second(request, resp);
        found = true;
      }
    }

    if (!found) {
      resp.set_code(404);
      resp.send_body("<h1>404: Page not found :C</h1>");
    }
  } catch (std::out_of_range) {
  } catch (TCP::Error) {
  }
}
} // namespace Rum::HTTP