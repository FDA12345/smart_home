SET MQTT_PUB="c:\Program files\mosquitto\mosquitto_pub"

rem %MQTT_PUB% -h narodmon.ru -p 1883 -u filinov_dmitry@mail.ru -P axvfev6hzk -i DACHA006C5B8FB796 -t fda123/DACHA006C5B8FB796/status -m online
%MQTT_PUB% -h narodmon.ru -p 1883 -u filinov_dmitry@mail.ru -P axvfev6hzk -i DACHA006C5B8FB796 -t fda123/DACHA006C5B8FB796/json -m "{""sensor1"":3,""sensor2"":4}"
