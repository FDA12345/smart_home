cmake_minimum_required(VERSION 3.10)

add_library(serial src/serial.cpp)

target_link_libraries(serial PRIVATE boost logger)
target_include_directories(serial PUBLIC include)

target_precompile_headers(serial PRIVATE src/stdafx.h)

set(HEADERS
	include/serial.h

	src/stdafx.h
)

set(SOURCES
	src/serial.cpp
)

target_sources(serial PRIVATE ${HEADERS} ${SOURCES})

