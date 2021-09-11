cmake_minimum_required(VERSION 3.10)

add_executable(wirenboard_svc src/main.cpp)

target_link_libraries(wirenboard_svc PRIVATE logger rapidjson wirenboard mqtt_broker broker_server boost curl)

target_precompile_headers(wirenboard_svc PRIVATE src/stdafx.h)

set(HEADERS
	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(wirenboard_svc PRIVATE ${HEADERS} ${SOURCES})

