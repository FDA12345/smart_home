#include <iostream>

#include <mqtt/async_client.h>

int main()
{
  mqtt::async_client cli("tcp://192.168.41.11:1883", "");

  std::cout << "hello world " << std::endl;
}