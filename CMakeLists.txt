cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(SDK "${CMAKE_SOURCE_DIR}/sdk")

if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
	set(PLATFORM "x64")
else()
	set(PLATFORM "x32")
endif()
message("Build platform is ${PLATFORM}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

if(WIN32)
	add_compile_options(-D_WIN32_WINNT=0x0500 -D_WIN32_WINDOWS=0x500)#_WIN32_WINNT_WIN2K
endif()

add_compile_options(-Zi)
add_link_options(-DEBUG)
add_compile_options("-MT$<$<CONFIG:Debug>:d>")

add_subdirectory(libs)
add_subdirectory(apps)