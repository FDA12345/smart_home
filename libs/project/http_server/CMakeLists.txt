cmake_minimum_required(VERSION 3.10)

add_library(http_server src/http_server.cpp)

#boost 1.73 inlining error in msvc 2017, so disabling one
if(WIN32)
	target_compile_options(http_server PRIVATE -Ob0)
endif()

target_link_libraries(http_server
	PUBLIC net_server
	PRIVATE boost logger
)

target_include_directories(http_server PUBLIC include)

target_precompile_headers(http_server PRIVATE src/stdafx.h)

set(HEADERS
	include/http_server.h

	src/stdafx.h
	src/http_session.h
	src/http_request.h
	src/http_response.h
)

set(SOURCES
	src/http_server.cpp
	src/http_session.cpp
	src/http_request.cpp
	src/http_response.cpp
)

target_sources(http_server PRIVATE ${HEADERS} ${SOURCES})

