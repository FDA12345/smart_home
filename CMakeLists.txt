cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(SDK "${CMAKE_SOURCE_DIR}/../smart_home_sdk")
#set(SDK "d:/fda/smart_home_sdk")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#add_compile_options(-MT)

#set(PAHO_BUILD_STATIC TRUE CACHE BOOL "Build static library")
#set(PAHO_HIGH_PERFORMANCE TRUE CACHE BOOL "Maximum optimization")
#add_subdirectory(vendors/paho.mqtt.c)

add_library(paho-mqtt3c-static UNKNOWN IMPORTED)
if(WIN32)
	set_property(TARGET paho-mqtt3c-static PROPERTY IMPORTED_LOCATION "${SDK}/paho.mqtt.c/build/src/Release/paho-mqtt3c-static.lib")
	target_link_libraries(paho-mqtt3c-static INTERFACE ws2_32)
else()
	set_property(TARGET paho-mqtt3c-static PROPERTY IMPORTED_LOCATION "${SDK}/paho.mqtt.c/build/src/Release/paho-mqtt3c-static.a")
endif()
target_include_directories(paho-mqtt3c-static INTERFACE "${SDK}/paho.mqtt.c/src")


add_subdirectory(src)