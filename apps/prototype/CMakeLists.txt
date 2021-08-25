cmake_minimum_required(VERSION 3.10)

add_executable(prototype src/main.cpp)

target_link_libraries(prototype PRIVATE noolite paho-mqtt-c boost logger broker net_server)

target_precompile_headers(prototype PRIVATE src/stdafx.h)

set(HEADERS

	src/stdafx.h
	src/MqttBroker.h
	src/BrokerNetServer.h
	src/HttpNetServer.h
)

set(SOURCES
	src/main.cpp
	src/MqttBroker.cpp
	src/BrokerNetServer.cpp
	src/HttpNetServer.cpp
)

target_sources(prototype PRIVATE ${HEADERS} ${SOURCES})

