add_library(Bbb SHARED Bbb.cpp)
target_link_libraries(Bbb PRIVATE Aaa)
target_include_directories(Bbb
  INTERFACE .
)
target_compile_definitions(Bbb
  PRIVATE -DBBB_LIB_COMPILING
)
