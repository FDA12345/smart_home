cmake_minimum_required(VERSION 3.10)

add_library(broker INTERFACE)

target_link_libraries(broker
#	INTERFACE net_server
)

target_include_directories(broker INTERFACE include)

#target_precompile_headers(http_server PRIVATE src/stdafx.h)

set(HEADERS
	include/broker.h

#	src/stdafx.h
)

set(SOURCES
#	src/http_server.cpp
)

target_sources(broker PRIVATE ${HEADERS} ${SOURCES})

