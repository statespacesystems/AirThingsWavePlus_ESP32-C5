#ifndef API_MQTT_HPP
#define API_MQTT_HPP

// #include <WiFi.h>
#include <PubSubClient.h>
#include "api_AirThingsWavePlus.hpp"

namespace api_mqtt 
{
class MqttPubSubPlus
{
public:
    MqttPubSubPlus();
    bool setupMqtt();
    void reconnect();
    void updateMqtt(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu);


private:
//static     void callback(char* topic, byte* payload, unsigned int length);
    long lastMsg;
    char msg[50];
    int value;
    float temperature;
    float humidity;

};

extern MqttPubSubPlus mqttPubSub;

} // namespace api_mqtt
#endif // API_MQTT_HPP