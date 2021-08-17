cmake_minimum_required(VERSION 3.10)

project(sms)

add_executable(sms src/main.cpp)

set_target_properties(sms PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#target_link_libraries(sms	
#	../../../../libs/paho.mqtt.c.static/lib/paho-mqtt3a-static
#	../../../../libs/paho.mqtt.cpp.static/lib/paho-mqttpp3-static
#)

#if (WIN32)
#	target_link_libraries(sms ws2_32)
#endif()

#target_include_directories(sms
#	PRIVATE "${PROJECT_BINARY_DIR}"
#	PRIVATE ../../../libs/paho.mqtt.c.static/include
#	PRIVATE ../../../libs/paho.mqtt.cpp.static/include
#)

#target_sources(sms PRIVATE
#	src/main.cpp
#)

