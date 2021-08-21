cmake_minimum_required(VERSION 3.10)

add_library(finglai_fths01 src/finglai_fths01.cpp)

target_link_libraries(finglai_fths01)
target_include_directories(finglai_fths01 PUBLIC include)

target_precompile_headers(finglai_fths01 PRIVATE src/stdafx.h)

set(HEADERS
	include/finglai_fths01.h

	src/stdafx.h
)

set(SOURCES
	src/finglai_fths01.cpp
)

target_sources(finglai_fths01 PRIVATE ${HEADERS} ${SOURCES})

