cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  obj2psx
  VERSION 0.1.0
  LANGUAGES CXX
)

add_executable(obj2psx
  obj2psx/src/main.cpp)
