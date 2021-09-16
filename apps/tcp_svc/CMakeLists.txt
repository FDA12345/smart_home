cmake_minimum_required(VERSION 3.10)

add_executable(tcp_svc src/main.cpp)

target_link_libraries(tcp_svc PRIVATE logger boost)

target_precompile_headers(tcp_svc PRIVATE src/stdafx.h)

set(HEADERS
	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(tcp_svc PRIVATE ${HEADERS} ${SOURCES})

