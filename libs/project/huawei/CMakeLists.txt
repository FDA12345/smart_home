cmake_minimum_required(VERSION 3.10)

add_library(huawei src/huawei.cpp)

target_link_libraries(huawei)
target_include_directories(huawei PUBLIC include)

target_precompile_headers(huawei PRIVATE src/stdafx.h)

set(HEADERS
	include/huawei.h

	src/stdafx.h
)

set(SOURCES
	src/huawei.cpp
)

target_sources(huawei PRIVATE ${HEADERS} ${SOURCES})

