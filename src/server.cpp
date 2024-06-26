#ifdef USE_FLAGS
#include <flags-cpp/flags-cpp.h>
#endif
#include <rum/http/http.h>
#include <rum/tcp/error.h>
#include <csignal>
#include <iostream>

Rum::HTTP::Server* server;

int main(int argc, char** argv) {
#ifdef USE_FLAGS

  Flags::Parser parser;
  int* port = parser.add<int>("port", "tcp port number", false, 8080);
  unsigned long int* workers = parser.add<unsigned long int>("workers", "number of worker threads", false, 10);

  bool* help = parser.add<bool>("help", "print help", false, false);

  if (!parser.parse(argc, argv) || *help) {
    parser.help();
    return -1;
  }

#else
  int* port = new int(argc >= 2 ? atoi(argv[1]) : 8080);
  unsigned long* workers = new unsigned long(10);
#endif

  std::cout << "Port: " << *port << std::endl;
  std::cout << "Workers: " << *workers << std::endl;

  try {
    server = new Rum::HTTP::Server(*port, *workers, DEFAULT_BUFFER_SIZE);

    std::signal(SIGINT, [](int) {
      std::cout << "\nStopping server..." << std::endl;
      server->end();
    });

    server->add_path<Rum::HTTP::GET>("/asd(/?|/.*)", [](const Rum::HTTP::Request& req, Rum::HTTP::Response& resp) {
      std::cout << "request accepted" << std::endl;
      resp.headers["Server"] = "Rum Barrel";
      resp.body = "<h1>asd</h1><pre>" + (std::string)req + "</pre>";
    });

    server->add_path<Rum::HTTP::GET>("/cookie", [](const Rum::HTTP::Request& req, Rum::HTTP::Response& resp) {
      std::cout << "request accepted" << std::endl;
      resp.cookies.push_back(Rum::HTTP::Cookie("testCookie", "valueOfCookie", "/", 60 * 60));
      resp.body = "<h1>Cookie</h1><pre>" + (std::string)req + "</pre>";
    });

    server->add_path<Rum::HTTP::POST>("/add", [](const Rum::HTTP::Request& req, Rum::HTTP::Response& resp) {
      std::cout << (std::string)req << std::endl;
      resp.body = "<h1>Hello World</h1><pre>" + (std::string)req + "</pre>";
    });

    server->add_path<Rum::HTTP::GET>("/.*", [](const Rum::HTTP::Request& req, Rum::HTTP::Response& resp) {
      std::cout << "request accepted" << std::endl;
      resp.body = "<h1>Hello World</h1><pre>" + (std::string)req + "</pre>";
    });

    server->listen();

    delete server;

  } catch (Rum::TCP::Error&) {
    std::cerr << "Failed to bind port " << *port << std::endl;
    return -1;
  }
}