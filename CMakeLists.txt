cmake_minimum_required(VERSION 3.0)
project(SmartHome)

set(PAHO_BUILD_STATIC TRUE)
add_subdirectory(vendors/paho.mqtt.c)

set(PAHO_MQTT_C_LIBRARIES)
set(PAHO_MQTT_C_INCLUDE_DIRS)
add_subdirectory(vendors/paho.mqtt.cpp)