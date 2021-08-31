cmake_minimum_required(VERSION 3.10)

add_executable(noolite_svc src/main.cpp)

target_link_libraries(noolite_svc PRIVATE noolite logger json_http_server)

target_precompile_headers(noolite_svc PRIVATE src/stdafx.h)

set(HEADERS

	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(noolite_svc PRIVATE ${HEADERS} ${SOURCES})

