#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <arduino.h>

namespace api_config
{

uint8_t  get_configRevision();

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
int16_t get_mqttPort();
char*   get_mqttTopicAirThings();
char*   get_mqttTopicAirThingsSubjectTemperature();
char*   get_mqttTopicAirThingsSubjectHumidity();
char*   get_mqttTopicAirThingsSubjectCO2();
char*   get_mqttTopicAirThingsSubjectLongTermRadon();
char*   get_mqttTopicAirThingsSubjectShortTermRadon();
char*   get_mqttTopicAirThingsSubjectPressure();
char*   get_mqttTopicAirThingsSubjectVoc();
char*   get_mqttTopicAirThingsSubjectTimeDate();

char*   get_mqtt_RtcTopic();

// Real Time Clock DS3231
bool    get_rtcUseAlarm();
bool    get_rtcUseSquareWave();
uint8_t get_rtcInterruptPin();

// Submodule enablers
bool    get_enable_AirThingsWavePlus();
bool    get_enable_rtcPlus();
bool    get_enable_cpuTemperature();
bool    get_enable_wiFiPlus();
bool    get_enable_mqtt();
bool    get_enable_webserver();
bool    get_enable_eepromAT24C32();
bool    get_enable_lcdDisplay();

bool readConfig();
bool writeConfig();

} // namespace api_config

#endif // API_CONFIG_H
