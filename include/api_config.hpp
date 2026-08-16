#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <arduino.h>

namespace api_config
{

// Airthings Wave Plus info
char*    get_ServiceUUID();
char*    get_CharUUID();
uint32_t get_AirThingsWavePlusSerialNumber();

// WiFi Credentials
char*   get_ssid();
char*   get_password();

// WiFi Settings:Static IP address, Gateway address, subnet and DNS
bool    get_useStaticIP();
uint8_t (&get_LocalIp())[4];
uint8_t (&get_Gateway())[4];
uint8_t (&get_Subnet())[4];
uint8_t (&get_PrimaryDNS())[4];
uint8_t (&get_SecondaryDNS())[4];

// MQTT Broker Settings
uint8_t (&get_mqtt_broker())[4];
int16_t get_mqtt_port();
char*   get_mqtt_AirThingsTopic();
char*   get_mqtt_RtcTopic();

// Real TIme Clock DS3231
bool    get_rtcUseAlarm();
bool    get_rtcUseSquareWave();
uint8_t get_rtcInterruptPin();

bool readConfig();
bool writeConfig();

} // namespace api_config

#endif // API_CONFIG_H
