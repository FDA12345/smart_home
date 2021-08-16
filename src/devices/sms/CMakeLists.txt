cmake_minimum_required(VERSION 3.10)

project(sms)

add_executable(sms src/main.cpp)

target_link_libraries(sms
	../../../../libs/paho-mqtt-c/lib/paho-mqtt3a-static
	../../../../libs/paho-mqtt-cpp/lib/paho-mqttpp3-static
)

if (WIN32)
	target_link_libraries(sms wsock32 ws2_32)
endif()

target_include_directories(sms
	PRIVATE "${PROJECT_BINARY_DIR}"
	PRIVATE ../../../libs/paho-mqtt-c/include
	PRIVATE ../../../libs/paho-mqtt-cpp/include
)

target_sources(sms PRIVATE
	src/main.cpp
)

