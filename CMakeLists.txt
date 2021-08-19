cmake_minimum_required(VERSION 3.15)
project(SmartHome)

set(SDK "${CMAKE_SOURCE_DIR}/sdk")



set(PAHO_MQTT_C paho.mqtt.c_1_3_9)

#set(BOOST "boost_1_77_0") #ошибка Inteliscense
set(BOOST "boost_1_73_0") #boost_1_74_0 + -> error in MSVC Inteliscense



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_compile_options(-Zi)
add_compile_options("-MT$<$<CONFIG:Debug>:d>")
add_link_options(-DEBUG)


add_library(paho-mqtt3c-static UNKNOWN IMPORTED)
if(WIN32)
	target_link_libraries(paho-mqtt3c-static INTERFACE ws2_32)
endif()
set_target_properties(paho-mqtt3c-static PROPERTIES
	IMPORTED_LOCATION "${SDK}/${PAHO_MQTT_C}/build/src/Release/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_LOCATION_DEBUG "${SDK}/${PAHO_MQTT_C}/build/src/Debug/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
target_include_directories(paho-mqtt3c-static INTERFACE "${SDK}/${PAHO_MQTT_C}/src")


add_library(boost INTERFACE IMPORTED)
if(WIN32)
	target_link_libraries(boost INTERFACE ws2_32)
endif()
set_target_properties(boost PROPERTIES
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
target_include_directories(boost INTERFACE "${SDK}/${BOOST}")
target_link_directories(boost INTERFACE "${SDK}/${BOOST}/stage/lib")
target_compile_options(boost INTERFACE "$<$<CONFIG:Debug>:-DBOOST_LIB_DIAGNOSTIC>")



add_subdirectory(src)