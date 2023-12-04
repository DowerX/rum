#include <flags/flags.h>
#include <rum/http/http.h>
#include <rum/tcp/error.h>
#include <csignal>
#include <iostream>

Rum::HTTP::Server* server;

int main(int argc, char** argv) {
  Flags::Parser parser;
  int* port = parser.add<int>("port", "tcp port number", true, 8080);
  parser.parse(argc, argv);

  std::cout << "Port: " << *port << std::endl;

  try {
    server = new Rum::HTTP::Server(*port);
    std::signal(SIGINT, [](int) {
      std::cout << "\nStopping server..." << std::endl;
      server->end();
    });
    server->add_path<Rum::HTTP::GET>("/", [](const Rum::HTTP::Request&, Rum::HTTP::Response& resp) {
      std::cout << "request accepted" << std::endl;
      resp.send_body("<h1>Hello World</h1>");
    });
    server->listen();
    delete server;
  } catch (Rum::TCP::Error&) {
    std::cerr << "Failed to bind port " << *port << std::endl;
  }
}