# Rum, a HTTP server library for C++
inspired by Golang libraries

## Usage
An example server is included in `src/server.cpp`.
```c++
try {
  // create a server           ˇthread count         ˇsocket reading buffer size
  Rum::HTTP::Server server(80, DEFAULT_WORKER_COUNT, DEFAULT_BUFFER_SIZE);

  // create a new path with GET method
  server->add_path<Rum::HTTP::GET>(
    "/.*",                                                                   // regex for the path
    [](const Rum::HTTP::Request& req, Rum::HTTP::Response& resp) {           // your handler function
      resp.body = "<h1>Hello World</h1><pre>" + (std::string)req + "</pre>"; // send some data
    }
  );

  // listen for connections
  server.listen();
} catch (Rum::TCP::Error&) {
  // failed to bind port
}
```

## Features
- thread pooling
- regex paths
- only system and standard dependencies