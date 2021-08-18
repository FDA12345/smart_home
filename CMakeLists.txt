cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(PAHO_BUILD_STATIC TRUE CACHE BOOL "Build static library")
set(PAHO_HIGH_PERFORMANCE TRUE CACHE BOOL "Maximum optimization")
add_subdirectory(vendors/paho.mqtt.c)

add_subdirectory(src)