cmake_minimum_required(VERSION 3.0)
project(SmartHome)

set(PAHO_BUILD_STATIC TRUE)
add_subdirectory(vendors/paho.mqtt.c)

#message(STATUS "${_variableName}=${${_variableName}}")
#set(ECLIPSE_PAHO_C_BINARY_DIR "Eclipse Paho C_BINARY_DIR")
#set(ECLIPSE_PAHO_C_SOURCE_DIR "Eclipse Paho C_SOURCE_DIR")
#message(STATUS "$ECLIPSE_PAHO_C_BINARY_DIR ${${ECLIPSE_PAHO_C_BINARY_DIR}}")
#message(STATUS "$ECLIPSE_PAHO_C_SOURCE_DIR ${${ECLIPSE_PAHO_C_SOURCE_DIR}}")

#set(CMAKE_PREFIX_PATH vendors/paho.mqtt.c)

#find_library(MQTT_C NAMES paho-mqtt3a PATHS vendors/paho.mqtt.c HINTS vendors/paho.mqtt.c REQUIRED)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#set(PAHO_MQTT_C_LIBRARIES ${${ECLIPSE_PAHO_C_BINARY_DIR}}/src/Debug/paho-mqtt3a.lib)
#set(PAHO_MQTT_C_INCLUDE_DIRS ${${ECLIPSE_PAHO_C_SOURCE_DIR}}/src)
add_subdirectory(vendors/paho.mqtt.cpp)