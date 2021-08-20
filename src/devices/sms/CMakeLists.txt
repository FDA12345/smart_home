cmake_minimum_required(VERSION 3.10)

add_executable(sms src/main.cpp)

target_link_libraries(sms PRIVATE paho-mqtt3c-static  boost)

target_precompile_headers(sms PRIVATE src/stdafx.h)

set(HEADERS
	src/common/Broker.h
	src/common/NetServer.h
	
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

target_sources(sms PRIVATE ${HEADERS} ${SOURCES})

