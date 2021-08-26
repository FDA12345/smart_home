cmake_minimum_required(VERSION 3.10)

add_library(http_server src/http_server.cpp)

target_link_libraries(http_server
	PUBLIC net_server
	PRIVATE boost logger
)

target_include_directories(http_server PUBLIC include)

target_precompile_headers(http_server PRIVATE src/stdafx.h)

set(HEADERS
	include/http_server.h

	src/stdafx.h
	src/http_session.h
)

set(SOURCES
	src/http_server.cpp
	src/http_session.cpp
)

target_sources(http_server PRIVATE ${HEADERS} ${SOURCES})

