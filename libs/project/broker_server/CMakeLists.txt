cmake_minimum_required(VERSION 3.10)

add_library(broker_server src/broker_server.cpp)

target_link_libraries(broker_server
	PUBLIC net_server
	PRIVATE logger broker
)

target_include_directories(broker_server PUBLIC include)

target_precompile_headers(broker_server PRIVATE src/stdafx.h)

set(HEADERS
	include/broker_server.h

	src/stdafx.h
)

set(SOURCES
	src/broker_server.cpp
)

target_sources(broker_server PRIVATE ${HEADERS} ${SOURCES})

