cmake_minimum_required(VERSION 3.10)

add_library(noolite src/main.cpp)

target_link_libraries(noolite PRIVATE boost)
target_include_directories(noolite PUBLIC include)

target_precompile_headers(noolite PRIVATE src/stdafx.h)

set(HEADERS
	include/noolite.h

	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(noolite PRIVATE ${HEADERS} ${SOURCES})

