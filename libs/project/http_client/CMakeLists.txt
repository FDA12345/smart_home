cmake_minimum_required(VERSION 3.10)

add_library(http_client src/http_client.cpp)

target_link_libraries(http_client
	PRIVATE logger curl
)

target_include_directories(http_client PUBLIC include)

target_precompile_headers(http_client PRIVATE src/stdafx.h)

set(HEADERS
	include/http_client.h

	src/stdafx.h
)

set(SOURCES
	src/http_client.cpp
)

target_sources(http_client PRIVATE ${HEADERS} ${SOURCES})

