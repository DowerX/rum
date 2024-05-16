cmake_minimum_required(VERSION 3.25)

project(Rum VERSION 1.0)

# compiler setup
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field")

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/rum/*.cpp")
set(HEADERS "${CMAKE_SOURCE_DIR}/inc/rum")
add_library(rum OBJECT ${SOURCES})
set_target_properties(rum PROPERTIES POSITION_INDEPENDENT_CODE 1)
target_include_directories(rum PUBLIC "${CMAKE_SOURCE_DIR}/inc")

add_library(rum-static STATIC $<TARGET_OBJECTS:rum>)
set_target_properties(rum-static PROPERTIES OUTPUT_NAME rum)
target_include_directories(rum-static PUBLIC "${CMAKE_SOURCE_DIR}/inc")

add_library(rum-shared SHARED $<TARGET_OBJECTS:rum>)
set_target_properties(rum-shared PROPERTIES OUTPUT_NAME rum)
target_include_directories(rum-shared PUBLIC "${CMAKE_SOURCE_DIR}/inc")

add_executable(example "${CMAKE_SOURCE_DIR}/src/server.cpp")
target_link_libraries(example rum-shared)
if(USE_FLAGS)
    target_link_libraries(example flags-cpp)
endif()

install(TARGETS
    rum-static
    rum-shared
    RUNTIME
        COMPONENT Runtime
    LIBRARY
        COMPONENT Runtime
    ARCHIVE
        COMPONENT Development
)
install(DIRECTORY ${HEADERS} DESTINATION include COMPONENT Development)