SET PAHO_MQTT_C=paho.mqtt.c_1_3_9

cmake -B %PAHO_MQTT_C%/build_x32 %PAHO_MQTT_C%
cmake --build %PAHO_MQTT_C%/build_x32 --config Debug
cmake --build %PAHO_MQTT_C%/build_x32 --config Release

cmake -DCMAKE_GENERATOR_PLATFORM=x64 -B %PAHO_MQTT_C%/build_x64 %PAHO_MQTT_C%
cmake --build %PAHO_MQTT_C%/build_x64 --config Debug
cmake --build %PAHO_MQTT_C%/build_x64 --config Release
