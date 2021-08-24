cmake_minimum_required(VERSION 3.10)

add_library(noolite src/dongle.cpp)

target_link_libraries(noolite PUBLIC serial PRIVATE logger)
target_include_directories(noolite PUBLIC include)

target_precompile_headers(noolite PRIVATE src/stdafx.h)

set(HEADERS
	include/noolite.h
	include/device_info.h
	include/dongle.h
	include/dongle_mode.h
	include/answer.h

	src/stdafx.h
	
	src/noolite/cmd.h
	src/noolite/control.h
	src/noolite/packet.h
	src/noolite/dongle_packet.h
)

set(SOURCES
	src/dongle.cpp
)

target_sources(noolite PRIVATE ${HEADERS} ${SOURCES})

