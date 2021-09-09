cmake_minimum_required(VERSION 3.10)

add_library(json_http_client src/json_http_client.cpp)

target_link_libraries(json_http_client
	PRIVATE logger rapidjson
)

target_include_directories(json_http_client PUBLIC include)

target_precompile_headers(json_http_client PRIVATE src/stdafx.h)

set(HEADERS
	include/json_http_client.h

	src/stdafx.h
)

set(SOURCES
	src/json_http_client.cpp
)

target_sources(json_http_client PRIVATE ${HEADERS} ${SOURCES})

