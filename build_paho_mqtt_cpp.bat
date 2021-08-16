rem SET ROOT_PATH=d:/fda/smart_home
SET BUILD_TYPE=Release


SET LIB_MQTT_C=paho.mqtt.c
SET LIB_MQTT_CPP=paho.mqtt.cpp


cd vendors/%LIB_MQTT_C%
cmake -B../../build.%LIB_MQTT_C%.static -H. -DCMAKE_INSTALL_PREFIX=../../libs/%LIB_MQTT_C%.static -DPAHO_BUILD_STATIC=TRUE
cmake --build ../../build.%LIB_MQTT_C%.static --target install --config %BUILD_TYPE%
cd ../..


cd vendors/%LIB_MQTT_CPP%
cmake -B../../build.%LIB_MQTT_CPP%.static -H. -DCMAKE_INSTALL_PREFIX=../../libs/%LIB_MQTT_CPP%.static -DPAHO_MQTT_C_LIBRARIES=../../libs/%LIB_MQTT_C%.static/lib/paho-mqtt3a -DPAHO_MQTT_C_INCLUDE_DIRS=../../libs/%LIB_MQTT_C%.static/include -DPAHO_BUILD_STATIC=TRUE
cmake --build ../../build.%LIB_MQTT_CPP%.static --target install --config %BUILD_TYPE%
cd ../..
