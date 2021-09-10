cmake -DCMAKE_GENERATOR_PLATFORM=x64 -B build_x64 

cmake --build build_x64 --config Debug
cmake --build build_x64 --config Release
