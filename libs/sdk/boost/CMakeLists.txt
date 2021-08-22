#set(BOOST "boost_1_77_0") #ошибка Inteliscense
set(BOOST "boost_1_73_0") #boost_1_74_0 + -> error in MSVC Inteliscense

add_library(boost INTERFACE IMPORTED GLOBAL)
if(WIN32)
	target_link_libraries(boost INTERFACE ws2_32)
endif()
set_target_properties(boost PROPERTIES
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
target_include_directories(boost INTERFACE "${SDK}/${BOOST}")
target_link_directories(boost INTERFACE "${SDK}/${BOOST}/stage/lib")
target_compile_options(boost INTERFACE "$<$<CONFIG:Debug>:-DBOOST_LIB_DIAGNOSTIC>")
