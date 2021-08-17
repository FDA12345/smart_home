cmake_minimum_required(VERSION 3.0)
project(SmartHome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(PAHO_BUILD_STATIC TRUE)
add_subdirectory(vendors/paho.mqtt.c)

add_subdirectory(src)