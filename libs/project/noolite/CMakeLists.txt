cmake_minimum_required(VERSION 3.10)

add_library(noolite src/noolite.cpp)

target_link_libraries(noolite PRIVATE serial)
target_include_directories(noolite PUBLIC include)

target_precompile_headers(noolite PRIVATE src/stdafx.h)

set(HEADERS
	include/noolite.h
	include/device_info.h
	include/dongle.h
	include/dongle_mode.h

	src/stdafx.h
	
	src/noolite/cmd.h
	src/noolite/control.h
	src/noolite/answer.h
	src/noolite/packet.h
)

set(SOURCES
	src/noolite.cpp
)

target_sources(noolite PRIVATE ${HEADERS} ${SOURCES})

