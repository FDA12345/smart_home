set(PAHO_MQTT_C paho.mqtt.c_1_3_9)

add_library(paho-mqtt-c UNKNOWN IMPORTED GLOBAL)
if(WIN32)
	target_link_libraries(paho-mqtt-c INTERFACE ws2_32)
endif()

if(PLATFORM STREQUAL "x32")
set_target_properties(paho-mqtt-c PROPERTIES
	IMPORTED_LOCATION "${SDK}/${PAHO_MQTT_C}/build_x32/src/Release/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_LOCATION_DEBUG "${SDK}/${PAHO_MQTT_C}/build_x32/src/Debug/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
else()
set_target_properties(paho-mqtt-c PROPERTIES
	IMPORTED_LOCATION "${SDK}/${PAHO_MQTT_C}/build_x64/src/Release/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_LOCATION_DEBUG "${SDK}/${PAHO_MQTT_C}/build_x64/src/Debug/paho-mqtt3c-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
endif()


target_include_directories(paho-mqtt-c INTERFACE "${SDK}/${PAHO_MQTT_C}/src")
