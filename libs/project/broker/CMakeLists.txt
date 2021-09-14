cmake_minimum_required(VERSION 3.10)

add_library(broker INTERFACE)

target_link_libraries(broker)

target_include_directories(broker INTERFACE include)

set(HEADERS
	include/broker.h
)

set(SOURCES
)

target_sources(broker PRIVATE ${HEADERS} ${SOURCES})

