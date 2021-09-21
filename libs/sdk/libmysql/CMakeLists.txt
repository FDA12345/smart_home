set(LIBMYSQL_PATH "${SDK}/libmysql-5.7.34")

add_library(mysql INTERFACE IMPORTED GLOBAL)

if(WIN32)
	target_link_libraries(mysql INTERFACE libmysql)
endif()
set_target_properties(mysql PROPERTIES
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)

target_include_directories(mysql INTERFACE "${LIBMYSQL_PATH}/${PLATFORM}/include")
target_link_directories(mysql INTERFACE "${LIBMYSQL_PATH}/${PLATFORM}/lib")
