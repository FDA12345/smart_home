cmake_minimum_required(VERSION 3.10)

add_executable(tags_svc src/main.cpp)

target_link_libraries(tags_svc PRIVATE logger mqtt_broker broker_server http_server boost mysql)

target_precompile_headers(tags_svc PRIVATE src/stdafx.h)

set(HEADERS
	src/stdafx.h
	src/db.h
	src/db_versioning.h
	src/mysql_db.h
	src/tags_db.h
	src/tags_db_ver_1.h
)

set(SOURCES
	src/main.cpp
	src/mysql_db.cpp
	src/db_versioning.cpp
	src/tags_db.cpp
)

target_sources(tags_svc PRIVATE ${HEADERS} ${SOURCES})

