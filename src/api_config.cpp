#include <arduino.h>
#include "config.h"
#include "api_config.hpp"
#include "api_eepromAT23C32.hpp"

namespace api_config
{

typedef enum
{
configFlag_loadConfigFromEEPROM = 0x01 << 0
} ConfigFlagMask;

typedef enum
{
enableFlag_AirThingsWavePlus    = 0x01 << 0,
enableFlag_rtcPlus              = 0x01 << 1,
enableFlag_cpuTemperature       = 0x01 << 2,
enableFlag_wiFiPlus             = 0x01 << 3,
enableFlag_mqtt                 = 0x01 << 4,
enableFlag_webserver            = 0x01 << 5,
enableFlag_eepromAT24C32        = 0x01 << 6
} EnableFlagMask;

typedef struct 
{
bool enable_AirThingsWavePlus;
bool enable_rtcPlus;
bool enable_cpuTemperature;
bool enable_wiFiPlus;
bool enable_mqtt;
bool enable_webserver;
bool enable_eepromAT24C32;
} EnableFlags;

typedef struct
{
char    AirThingsWavePlusServiceUUID[80];
char    AirThingsWavePlusCharUUID[80];
uint32_t AirThingsWavePlusSerialNumber[AirThingsWavePlusSerialNumberArraySize];
char    wiFiSsid[40];
char    wiFiPassword[40];
bool    wiFiUseStaticIP;
uint8_t wiFiLocalIp[4];
uint8_t wiFiGateway[4];
uint8_t wiFiSubnet[4];
uint8_t wiFiPrimaryDNS[4];
uint8_t wiFiSecondaryDNS[4];
uint8_t mqttBroker[4];
int16_t mqttPort;
char    mqttTopicAirThings[80];
char    mqttTopicRtc[80];
char    mqttTopicConfig[80];
bool    rtcUseAlarm;
bool    rtcUseSquareWave;
bool    rtcInterruptPin;
} Config;

static Config configDataBase;
static EnableFlags enableFlags = {false, false, false, false, false, false, false};
static uint8_t AirThingsWavePlusActiveID = 0;
static uint8_t eepromEnableRegister      = 0;
static uint8_t eepromConfigRegister      = 0;
static bool loadConfigFromEEPROM = config::loadConfigFromEEPROM;

bool readConfigFromCONFIG_H();
bool readConfigFromEEPROM();
bool writeConfigFromEEPROM();
bool isEepromConfigValid();

// From Struct
char*    get_ServiceUUID() {return (char*)configDataBase.AirThingsWavePlusServiceUUID;}
char*    get_CharUUID() {return (char*)configDataBase.AirThingsWavePlusCharUUID;}
uint8_t  get_AirThingsWavePlusActiveID() {return (uint32_t)AirThingsWavePlusActiveID;}
uint32_t get_AirThingsWavePlusSerialNumber() {return (uint32_t)configDataBase.AirThingsWavePlusSerialNumber[AirThingsWavePlusActiveID];}

char*   get_ssid() {return (char*)configDataBase.wiFiSsid;}
char*   get_password() {return (char*)configDataBase.wiFiPassword;}

bool    get_useStaticIP()      {return (bool)configDataBase.wiFiUseStaticIP;}
uint8_t (&get_LocalIp())[4]    {return (uint8_t (&)[4])configDataBase.wiFiLocalIp;}
uint8_t (&get_Gateway())[4]    {return (uint8_t (&)[4])configDataBase.wiFiGateway;}
uint8_t (&get_Subnet())[4]     {return (uint8_t (&)[4])configDataBase.wiFiSubnet;}
uint8_t (&get_PrimaryDNS())[4] {return (uint8_t (&)[4])configDataBase.wiFiPrimaryDNS;}
uint8_t (&get_SecondaryDNS())[4] {return (uint8_t (&)[4])configDataBase.wiFiSecondaryDNS;}

uint8_t (&get_mqttbroker())[4] {return (uint8_t (&)[4])configDataBase.mqttBroker;}
int16_t get_mqttport()         {return (int16_t)configDataBase.mqttPort;}
char*   get_mqttTopicAirThings() {return (char*)configDataBase.mqttTopicAirThings;}
char*   get_mqttTopicRtc()       {return (char*)configDataBase.mqttTopicRtc;}
char*   get_mqttTopicConfig()    {return (char*)configDataBase.mqttTopicConfig;}

bool    get_rtcUseAlarm()       {return (bool)configDataBase.rtcUseAlarm;}
bool    get_rtcUseSquareWave()  {return (bool)configDataBase.rtcUseSquareWave;}
uint8_t get_rtcInterruptPin()   {return (uint8_t)configDataBase.rtcInterruptPin;}

// From config file
char*    _get_ServiceUUID() {return (char*)config::AirThingsWavePlusServiceUUID;}
char*    _get_CharUUID() {return (char*)config::AirThingsWavePlusCharUUID;}
uint8_t  _get_AirThingsWavePlusActiveID() {return (uint32_t)config::AirThingsWavePlusActiveID;}
uint32_t _get_AirThingsWavePlusSerialNumber() {return (uint32_t)config::AirThingsWavePlusSerialNumber[AirThingsWavePlusActiveID];}

char*   _get_ssid() {return (char*)config::wiFiSsid;}
char*   _get_password() {return (char*)config::wiFiPassword;}

bool    _get_useStaticIP()      {return (bool)config::wiFiUseStaticIP;}
uint8_t (&_get_LocalIp())[4]    {return (uint8_t (&)[4])config::wiFiLocalIp;}
uint8_t (&_get_Gateway())[4]    {return (uint8_t (&)[4])config::wiFiGateway;}
uint8_t (&_get_Subnet())[4]     {return (uint8_t (&)[4])config::wiFiSubnet;}
uint8_t (&_get_PrimaryDNS())[4] {return (uint8_t (&)[4])config::wiFiPrimaryDNS;}
uint8_t (&_get_SecondaryDNS())[4] {return (uint8_t (&)[4])config::wiFiSecondaryDNS;}

uint8_t (&_get_mqttbroker())[4] {return (uint8_t (&)[4])config::mqttBroker;}
int16_t _get_mqttport()         {return (int16_t)config::mqttPort;}
char*   _get_mqttTopicAirThings() {return (char*)config::mqttTopicAirThings;}
char*   _get_mqttTopicRtc()       {return (char*)config::mqttTopicRtc;}
char*   _get_mqttTopicConfig()    {return (char*)config::mqttTopicConfig;}

bool    _get_rtcUseAlarm()       {return (bool)config::rtcUseAlarm;}
bool    _get_rtcUseSquareWave()  {return (bool)config::rtcUseSquareWave;}
uint8_t _get_rtcInterruptPin()   {return (uint8_t)config::rtcInterruptPin;}

bool readConfig()
{
    memset(&configDataBase, 0x00, sizeof(configDataBase));

    if (loadConfigFromEEPROM)
    {
        return readConfigFromEEPROM();
    }
    else
    {
        return readConfigFromCONFIG_H();
    }
}

bool readConfigFromCONFIG_H()
{
    uint8_t ActiveID = config::AirThingsWavePlusActiveID;
    AirThingsWavePlusActiveID = (ActiveID < AirThingsWavePlusSerialNumberArraySize) ? ActiveID : 0;

    configDataBase.wiFiUseStaticIP = config::wiFiUseStaticIP;
    configDataBase.mqttPort = config::mqttPort;
    configDataBase.rtcUseAlarm = config::rtcUseAlarm;
    configDataBase.rtcUseSquareWave = config::rtcUseSquareWave;
    configDataBase.rtcInterruptPin = config::rtcInterruptPin;

    strncpy(configDataBase.AirThingsWavePlusServiceUUID, config::AirThingsWavePlusServiceUUID, sizeof(configDataBase.AirThingsWavePlusServiceUUID) - 1);
    configDataBase.AirThingsWavePlusServiceUUID[sizeof(configDataBase.AirThingsWavePlusServiceUUID) - 1] = '\0';

    strncpy(configDataBase.AirThingsWavePlusCharUUID, config::AirThingsWavePlusCharUUID, sizeof(configDataBase.AirThingsWavePlusCharUUID) - 1);
    configDataBase.AirThingsWavePlusCharUUID[sizeof(configDataBase.AirThingsWavePlusCharUUID) - 1] = '\0';

    strncpy(configDataBase.wiFiSsid, config::wiFiSsid, sizeof(configDataBase.wiFiSsid) - 1);
    configDataBase.wiFiSsid[sizeof(configDataBase.wiFiSsid) - 1] = '\0';

    strncpy(configDataBase.wiFiPassword, config::wiFiPassword, sizeof(configDataBase.wiFiPassword) - 1);
    configDataBase.wiFiPassword[sizeof(configDataBase.wiFiPassword) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThings, config::mqttTopicAirThings, sizeof(configDataBase.mqttTopicAirThings) - 1);
    configDataBase.mqttTopicAirThings[sizeof(configDataBase.mqttTopicAirThings) - 1] = '\0';

    strncpy(configDataBase.mqttTopicRtc, config::mqttTopicRtc, sizeof(configDataBase.mqttTopicRtc) - 1);
    configDataBase.mqttTopicRtc[sizeof(configDataBase.mqttTopicRtc) - 1] = '\0';

    strncpy(configDataBase.mqttTopicConfig, config::mqttTopicConfig, sizeof(configDataBase.mqttTopicConfig) - 1);
    configDataBase.mqttTopicConfig[sizeof(configDataBase.mqttTopicConfig) - 1] = '\0';

    for (int i = 0; i < AirThingsWavePlusSerialNumberArraySize; i++)
    {
        configDataBase.AirThingsWavePlusSerialNumber[i] = config::AirThingsWavePlusSerialNumber[i];
    }

    for (int i = 0; i < 4; i++)
    {
        configDataBase.wiFiLocalIp[i]      = config::wiFiLocalIp[i];;
        configDataBase.wiFiGateway[i]      = config::wiFiGateway[i];
        configDataBase.wiFiSubnet[i]       = config::wiFiSubnet[i];
        configDataBase.wiFiPrimaryDNS[i]   = config::wiFiPrimaryDNS[i];
        configDataBase.wiFiSecondaryDNS[i] = config::wiFiSecondaryDNS[i];
        configDataBase.mqttBroker[i]       = config::mqttBroker[i];
    }

    Serial.println("Config database read from config.h");
    return true;
}

bool readConfigFromEEPROM()
{
    bool retValue = false;
    bool isConfigInEepromValid = isEepromConfigValid();

    Serial.print("isConfigInEepromValid: "); Serial.println(isConfigInEepromValid);

    if (isConfigInEepromValid)
    {
        uint8_t ActiveID = api_eeprom::eeprom.getAirThingsWavePlusActiveID();
        AirThingsWavePlusActiveID = (ActiveID < AirThingsWavePlusSerialNumberArraySize) ? ActiveID : 0;
        Serial.print("ActiveID: "); Serial.println(ActiveID);

        eepromConfigRegister = api_eeprom::eeprom.getConfigRegister();

        eepromEnableRegister = api_eeprom::eeprom.getEnableRegister();
        memset(&enableFlags, 0x00, sizeof(enableFlags));
        enableFlags.enable_AirThingsWavePlus = ((eepromEnableRegister & enableFlag_AirThingsWavePlus) == enableFlag_AirThingsWavePlus);
        enableFlags.enable_rtcPlus           = ((eepromEnableRegister & enableFlag_rtcPlus) == enableFlag_rtcPlus);
        enableFlags.enable_cpuTemperature    = ((eepromEnableRegister & enableFlag_cpuTemperature) == enableFlag_cpuTemperature);
        enableFlags.enable_wiFiPlus          = ((eepromEnableRegister & enableFlag_wiFiPlus) == enableFlag_wiFiPlus);
        enableFlags.enable_mqtt              = ((eepromEnableRegister & enableFlag_mqtt) == enableFlag_mqtt);
        enableFlags.enable_webserver         = ((eepromEnableRegister & enableFlag_webserver) == enableFlag_webserver);
        enableFlags.enable_eepromAT24C32     = ((eepromEnableRegister & enableFlag_eepromAT24C32) == enableFlag_eepromAT24C32);


        memset(&configDataBase, 0x00, sizeof(configDataBase));
        Serial.println("Config database read from EEPROM");
        retValue = true;
    }
    else
    {
        Serial.println("EEPROM not configured, defaulting to config.h");
        retValue = readConfigFromCONFIG_H();
    }

    return retValue;
}

bool writeConfigFromEEPROM()
{
    return true;
}

bool isEepromConfigValid()
{
    uint8_t configRegister = api_eeprom::eeprom.getConfigRegister();
    return ((configRegister & configFlag_loadConfigFromEEPROM) == configFlag_loadConfigFromEEPROM);
}

} // namespace api_config
