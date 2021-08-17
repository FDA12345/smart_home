cmake_minimum_required(VERSION 3.0)
project(DDD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_subdirectory(Aaa)
add_subdirectory(Bbb)
add_subdirectory(CCC)
