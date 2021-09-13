cd curl-7.78.0
mkdir curl_build
cd curl_build

cmake -DCMAKE_USE_SCHANNEL=ON .. -B build_x32

cmake --build build_x32 --config Debug
cmake --build build_x32 --config Release

cmake -DCMAKE_USE_SCHANNEL=ON -DCMAKE_GENERATOR_PLATFORM=x64 .. -B build_x64

cmake --build build_x64 --config Debug
cmake --build build_x64 --config Release

cd ..\..
