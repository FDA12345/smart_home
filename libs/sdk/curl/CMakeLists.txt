set(CURL curl-7.78.0)

add_library(curl UNKNOWN IMPORTED GLOBAL)
if(WIN32)
	target_link_libraries(curl INTERFACE ws2_32)
endif()

target_compile_options(curl INTERFACE "-DCURL_STATICLIB")

set_target_properties(curl PROPERTIES
	IMPORTED_LOCATION "${SDK}/${CURL}/curl_build/build_x32/lib/Release/libcurl${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_LOCATION_DEBUG "${SDK}/${CURL}/curl_build/build_x32/lib/Debug/libcurl-d${CMAKE_STATIC_LIBRARY_SUFFIX}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
target_include_directories(curl INTERFACE "${SDK}/${CURL}/include")
