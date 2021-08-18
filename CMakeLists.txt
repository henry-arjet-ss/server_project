cmake_minimum_required(VERSION 3.10)

# set the project name
project(server.out)

find_package(cpprestsdk REQUIRED)


# add the executable
add_executable(server.out server.cpp)
target_link_libraries(server.out PRIVATE cpprestsdk::cpprest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./build")