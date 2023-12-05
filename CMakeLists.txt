cmake_minimum_required(VERSION 3.25)

project(Rum VERSION 1.0)

# compiler setup
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror") # warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")            # std c++23

if (STATIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")             # static
endif()

if (DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")                  # debug symbols
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")                   # optimisations

# IDE setup (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(server.bin ${SOURCES})

target_include_directories(server.bin PRIVATE "${CMAKE_SOURCE_DIR}/inc")

add_custom_target(run
  COMMAND $<TARGET_FILE:server.bin>
  DEPENDS server.bin
  COMMENT "Running server.bin"
)

add_custom_target(docker
  COMMAND docker build -t dowerx/rum:${CMAKE_PROJECT_VERSION} ..
  COMMENT "Building docker image"
)