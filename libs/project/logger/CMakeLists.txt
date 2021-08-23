cmake_minimum_required(VERSION 3.10)

add_library(logger src/logger.cpp)

target_link_libraries(logger)
target_include_directories(logger PUBLIC include)

target_precompile_headers(logger PRIVATE src/stdafx.h)

set(HEADERS
	include/logger.h

	src/stdafx.h
)

set(SOURCES
	src/logger.cpp
)

target_sources(logger PRIVATE ${HEADERS} ${SOURCES})

