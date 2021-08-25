cmake_minimum_required(VERSION 3.10)

add_library(net_server INTERFACE)

target_link_libraries(net_server)

target_include_directories(net_server INTERFACE include)

#target_precompile_headers(net_server PRIVATE src/stdafx.h)

set(HEADERS
	include/net_server.h

#	src/stdafx.h
)

set(SOURCES
#	src/http_server.cpp
)

target_sources(net_server PRIVATE ${HEADERS} ${SOURCES})

