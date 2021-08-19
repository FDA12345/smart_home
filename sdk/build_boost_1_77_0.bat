pushd .
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
popd
cd boost_1_77_0
call bootstrap.bat
rem b2.exe --build-type=complete -j6 debug-symbols=on numa=on stage
b2.exe --build-type=complete -j6 debug-symbols=on stage
cd ..
