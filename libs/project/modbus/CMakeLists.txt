cmake_minimum_required(VERSION 3.10)

add_library(modbus src/modbus.cpp)

target_link_libraries(modbus PUBLIC serial)
target_include_directories(modbus PUBLIC include)

target_precompile_headers(modbus PRIVATE src/stdafx.h)

set(HEADERS
	include/modbus.h

	src/stdafx.h
)

set(SOURCES
	src/modbus.cpp
)

target_sources(modbus PRIVATE ${HEADERS} ${SOURCES})

