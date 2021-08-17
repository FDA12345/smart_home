cmake_minimum_required(VERSION 3.10)

add_executable(sms src/main.cpp)

if (PAHO_BUILD_STATIC)
	target_link_libraries(sms	
		PRIVATE paho-mqtt3a-static
		PRIVATE paho-mqtt3c-static
	)
else()
	target_link_libraries(sms	
		PRIVATE paho-mqtt3a
		PRIVATE paho-mqtt3c
	)
endif()

target_precompile_headers(sms PRIVATE src/stdafx.h)

#target_include_directories(sms PRIVATE src)

target_sources(sms PRIVATE
	src/stdafx.h
	src/main.cpp

	src/Broker.h
	src/MqttBroker.h
	src/MqttBroker.cpp
)

