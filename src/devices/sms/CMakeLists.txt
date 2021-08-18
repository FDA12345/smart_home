cmake_minimum_required(VERSION 3.10)

add_executable(sms src/main.cpp)

target_link_libraries(sms PRIVATE paho-mqtt3c-static)
target_compile_options(sms PRIVATE -MT)

target_precompile_headers(sms PRIVATE src/stdafx.h)

target_sources(sms PRIVATE
	src/stdafx.h
	src/main.cpp

	src/Broker.h
	src/MqttBroker.h
	src/MqttBroker.cpp
)

