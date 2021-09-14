cmake_minimum_required(VERSION 3.10)

add_library(messages src/messages.cpp)

target_link_libraries(messages PRIVATE rapidjson)
target_include_directories(messages PUBLIC include)

target_precompile_headers(messages PRIVATE src/stdafx.h)

set(HEADERS
	include/messages.h

	src/stdafx.h
)

set(SOURCES
	src/messages.cpp
)

target_sources(messages PRIVATE ${HEADERS} ${SOURCES})

