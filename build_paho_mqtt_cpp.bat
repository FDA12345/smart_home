SET INSTALL_PATH=d:/fda/smart_home
SET BUILD_TYPE=Release

cd %INSTALL_PATH%/vendors/paho.mqtt.c
cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH%/libs/paho-mqtt-c -DPAHO_BUILD_STATIC=TRUE
cmake --build build/ --target install --config %BUILD_TYPE%
cd %INSTALL_PATH%

cd %INSTALL_PATH%/vendors/paho.mqtt.c
cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH%/libs/paho-mqtt-c
cmake --build build/ --target install --config %BUILD_TYPE%
cd %INSTALL_PATH%

cd %INSTALL_PATH%/vendors/paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=TRUE -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH%/libs/paho-mqtt-cpp -DPAHO_BUILD_SAMPLES=OFF -DPAHO_WITH_SSL=OFF -DCMAKE_PREFIX_PATH=%INSTALL_PATH%/libs/paho-mqtt-c
cmake --build build/ --target install --config %BUILD_TYPE%
cd %INSTALL_PATH%

cd %INSTALL_PATH%/vendors/paho.mqtt.cpp
cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH%/libs/paho-mqtt-cpp -DPAHO_BUILD_SAMPLES=OFF -DPAHO_WITH_SSL=OFF -DCMAKE_PREFIX_PATH=%INSTALL_PATH%/libs/paho-mqtt-c
cmake --build build/ --target install --config %BUILD_TYPE%
cd %INSTALL_PATH%
