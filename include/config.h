#ifndef CONFIG_H
#define CONFIG_H

#include <arduino.h>

namespace config
{

// Config locator
const bool     loadConfigFromEEPROM = true;

// Airthings Wave Plus info
#define AirThingsWavePlusSerialNumberArraySize 5
const uint8_t AirThingsWavePlusActiveID    = 1;
const char*   AirThingsWavePlusServiceUUID = "b42e1c08-ade7-11e4-89d3-123b93f75cba";
const char*   AirThingsWavePlusCharUUID    = "b42e2a68-ade7-11e4-89d3-123b93f75cba";
const uint32_t AirThingsWavePlusSerialNumber[AirThingsWavePlusSerialNumberArraySize] = {(uint32_t)2930210229, (uint32_t)2930214197, 0, 0, 0};

// WiFi Credentials
#define WIFI_SSID     "Your SSID"
#define WIFI_PASSWORD "Your Password"
#include "WiFiCredentials.h"
const char* wiFiSsid               = WIFI_SSID;
const char* wiFiPassword           = WIFI_PASSWORD;

// WiFi Settings:Static IP address, Gateway address, subnet and DNS
const bool    wiFiUseStaticIP      = true;
const uint8_t wiFiLocalIp[4]       = {192, 168, 1, 22};
const uint8_t wiFiGateway[4]       = {192, 168, 1, 1};
const uint8_t wiFiSubnet[4]        = {255, 255, 255, 0};
const uint8_t wiFiPrimaryDNS[4]    = {8, 8, 8, 8};
const uint8_t wiFiSecondaryDNS[4]  = {8, 8, 4, 4};

// MQTT Broker Settings
const uint8_t mqttBroker[4]       = {192, 168, 1, 66};
const int16_t mqttPort            = 1883;
const char*   mqttTopicAirThings  = "airThingsWavePlus";
const char*   mqttTopicRtc        = "rtc";
const char*   mqttTopicConfig     = "config";

// Real TIme Clock DS3231
const bool rtcUseAlarm            = false;
const bool rtcUseSquareWave       = false;
const bool rtcInterruptPin        = D2;    // pin that is connected to SQW

} // namespace config

#endif
