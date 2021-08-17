add_executable(Ccc Ccc.cpp)
target_link_libraries(Ccc PRIVATE Bbb)
set_target_properties(Ccc PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
