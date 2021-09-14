cmake_minimum_required(VERSION 3.10)

add_executable(fths01_svc src/main.cpp)

target_link_libraries(fths01_svc PRIVATE logger finglai_fths01 mqtt_broker broker_server http_client messages boost)

target_precompile_headers(fths01_svc PRIVATE src/stdafx.h)

set(HEADERS
	src/stdafx.h
)

set(SOURCES
	src/main.cpp
)

target_sources(fths01_svc PRIVATE ${HEADERS} ${SOURCES})

