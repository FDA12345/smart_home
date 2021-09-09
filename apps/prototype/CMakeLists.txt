cmake_minimum_required(VERSION 3.10)

add_executable(prototype src/main.cpp)

target_link_libraries(prototype PRIVATE noolite logger json_http_server mqtt_broker)

target_precompile_headers(prototype PRIVATE src/stdafx.h)

set(HEADERS

	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(prototype PRIVATE ${HEADERS} ${SOURCES})

