cmake_minimum_required(VERSION 3.10)

add_executable(noolite_svc src/main.cpp)

target_link_libraries(noolite_svc PRIVATE noolite logger json_http_server rapidjson)

target_precompile_headers(noolite_svc PRIVATE src/stdafx.h)

set(HEADERS
	src/stdafx.h
	src/json_object.h
)

set(SOURCES
	src/main.cpp
	src/json_object.cpp
)

target_sources(noolite_svc PRIVATE ${HEADERS} ${SOURCES})

