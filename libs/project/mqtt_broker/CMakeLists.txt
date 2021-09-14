cmake_minimum_required(VERSION 3.10)

add_library(mqtt_broker)

target_link_libraries(mqtt_broker
	PUBLIC broker 
	PRIVATE logger paho-mqtt-c
)

target_include_directories(mqtt_broker PUBLIC include)

target_precompile_headers(mqtt_broker PRIVATE src/stdafx.h)

set(HEADERS
	include/mqtt_broker.h

	src/stdafx.h
)

set(SOURCES
	src/mqtt_broker.cpp
)

target_sources(mqtt_broker PRIVATE ${HEADERS} ${SOURCES})

