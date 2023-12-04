cmake_minimum_required(VERSION 3.25)

project(Rum VERSION 1.0)

# compiler setup
set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror") # warnings and errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")            # std c++23 for clang

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static") # static compile
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")      # debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")       # enable optimisations


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