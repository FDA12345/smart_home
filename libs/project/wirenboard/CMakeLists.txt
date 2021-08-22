cmake_minimum_required(VERSION 3.10)

add_library(wirenboard src/wirenboard.cpp)

target_link_libraries(wirenboard)
target_include_directories(wirenboard PUBLIC include)

target_precompile_headers(wirenboard PRIVATE src/stdafx.h)

set(HEADERS
	include/wirenboard.h

	src/stdafx.h
)

set(SOURCES
	src/wirenboard.cpp
)

target_sources(wirenboard PRIVATE ${HEADERS} ${SOURCES})

