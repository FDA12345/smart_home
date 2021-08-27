cmake_minimum_required(VERSION 3.10)

add_library(json_http_server src/json_http_server.cpp)

target_link_libraries(json_http_server
	PUBLIC http_server
	PRIVATE logger rapidjson
)

target_include_directories(json_http_server PUBLIC include)

target_precompile_headers(json_http_server PRIVATE src/stdafx.h)

set(HEADERS
	include/json_http_server.h

	src/stdafx.h
)

set(SOURCES
	src/json_http_server.cpp
)

target_sources(json_http_server PRIVATE ${HEADERS} ${SOURCES})

