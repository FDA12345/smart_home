cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(SDK "${CMAKE_SOURCE_DIR}/sdk")
#set(SDK "${CMAKE_SOURCE_DIR}/../smart_home_sdk")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_compile_options(-MT)# -Zi)
#add_link_options(-DEBUG)


add_library(paho-mqtt3c-static UNKNOWN IMPORTED)

if(WIN32)
	target_link_libraries(paho-mqtt3c-static INTERFACE ws2_32)
endif()

set_target_properties(paho-mqtt3c-static PROPERTIES
	IMPORTED_LOCATION "${SDK}/paho.mqtt.c/build/src/Release/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_LOCATION_DEBUG "${SDK}/paho.mqtt.c/build/src/Debug/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)

target_include_directories(paho-mqtt3c-static INTERFACE "${SDK}/paho.mqtt.c/src")


add_subdirectory(src)