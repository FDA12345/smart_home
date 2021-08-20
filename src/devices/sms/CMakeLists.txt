cmake_minimum_required(VERSION 3.10)

add_executable(sms src/main.cpp)

target_link_libraries(sms PRIVATE paho-mqtt3c-static  boost)

target_precompile_headers(sms PRIVATE src/stdafx.h)

set(HEADERS
	src/common/Broker.h
	src/common/DlgServer.h
	
	src/stdafx.h
	src/MqttBroker.h
	src/BrokerDlgServer.h
	src/HttpDlgServer.h
)

set(SOURCES
	src/main.cpp
	src/MqttBroker.cpp
	src/BrokerDlgServer.cpp
	src/HttpDlgServer.cpp
)

target_sources(sms PRIVATE ${HEADERS} ${SOURCES})

