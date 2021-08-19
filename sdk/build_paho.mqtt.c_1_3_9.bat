SET PAHO_MQTT_C=paho.mqtt.c_1_3_9

cmake -B %PAHO_MQTT_C%/build %PAHO_MQTT_C%
cmake --build %PAHO_MQTT_C%/build --config Debug
cmake --build %PAHO_MQTT_C%/build --config Release
