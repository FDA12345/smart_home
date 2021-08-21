cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(SDK "${CMAKE_SOURCE_DIR}/sdk")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_compile_options(-Zi)
add_compile_options("-MT$<$<CONFIG:Debug>:d>")
add_link_options(-DEBUG)

add_subdirectory(libs)
add_subdirectory(apps)