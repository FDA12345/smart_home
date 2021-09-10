mkdir curl-7.78.0/curl_build
cd curl-7.78.0/curl_build

cmake .. -B build_x32

cmake --build build_x32 --config Debug
cmake --build build_x32 --config Release

cmake -DCMAKE_GENERATOR_PLATFORM=x64 .. -B build_x64

cmake --build build_x64 --config Debug
cmake --build build_x64 --config Release

cd ..\..
