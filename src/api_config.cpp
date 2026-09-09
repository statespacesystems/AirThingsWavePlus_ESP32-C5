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

typedef struct 
{
    bool config_loadConfigFromEEPROM;
} ConfigFlags;

typedef enum
{
enableFlag_AirThingsWavePlus    = 0x01 << 0,
enableFlag_rtcPlus              = 0x01 << 1,
enableFlag_cpuTemperature       = 0x01 << 2,
enableFlag_wiFiPlus             = 0x01 << 3,
enableFlag_mqtt                 = 0x01 << 4,
enableFlag_webserver            = 0x01 << 5,
enableFlag_eepromAT24C32        = 0x01 << 6,
enableFlag_lcdDisplay           = 0x01 << 7
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
bool enable_lcdDisplay;
} EnableFlags;

typedef struct
{
uint8_t revision;
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
char    mqttTopicAirThings[20];
char    mqttTopicAirThingsSubjectTemperature[20];
char    mqttTopicAirThingsSubjectHumidity[20];
char    mqttTopicAirThingsSubjectCO2[20];
char    mqttTopicAirThingsSubjectLongTermRadon[20];
char    mqttTopicAirThingsSubjectShortTermRadon[20];
char    mqttTopicAirThingsSubjectPressure[20];
char    mqttTopicAirThingsSubjectVoc[20];
char    mqttTopicAirThingsSubjectTimeDate[20];
char    mqttTopicRtc[20];
char    mqttTopicConfig[20];
bool    rtcUseAlarm;
bool    rtcUseSquareWave;
bool    rtcInterruptPin;
} Config;

static Config      configDataBase;
static ConfigFlags configFlags = {false};
static EnableFlags enableFlags = {false, false, false, false, false, false, false, false};
static uint8_t     AirThingsWavePlusActiveID = 0;
static uint8_t     eepromEnableRegister      = 0;
static uint8_t     eepromConfigRegister      = 0;
static bool        loadConfigFromEEPROM = config::loadConfigFromEEPROM;

bool readConfigFromCONFIG_H();
bool readConfigFromEEPROM();
bool writeConfigFromEEPROM();
bool isEepromConfigValid();

// Private functions
uint8_t mapEnableStructToEnableRegister();
void    mapEnablegRegisterToEnableStruct(uint8_t eepromEnableRegister);
uint8_t mapConfigStructToConfigRegister();
 void   mapConfigRegisterToConfigStruct(uint8_t eepromConfigRegister);

// From Struct
uint8_t  get_configRevision() {return (uint8_t)configDataBase.revision;}

char*    get_ServiceUUID() {return (char*)configDataBase.AirThingsWavePlusServiceUUID;}
char*    get_CharUUID() {return (char*)configDataBase.AirThingsWavePlusCharUUID;}
uint32_t get_AirThingsWavePlusActiveID() {return (uint32_t)AirThingsWavePlusActiveID;}
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
int16_t get_mqttPort()         {return (int16_t)configDataBase.mqttPort;}

// MQTT Air Things Wave Plus topics and categories
char*   get_mqttTopicAirThings() {return (char*)configDataBase.mqttTopicAirThings;}
char*   get_mqttTopicAirThingsSubjectTemperature() {return (char*)configDataBase.mqttTopicAirThingsSubjectTemperature;}
char*   get_mqttTopicAirThingsSubjectHumidity() {return (char*)configDataBase.mqttTopicAirThingsSubjectHumidity;}
char*   get_mqttTopicAirThingsSubjectCO2() {return (char*)configDataBase.mqttTopicAirThingsSubjectCO2;}
char*   get_mqttTopicAirThingsSubjectLongTermRadon() {return (char*)configDataBase.mqttTopicAirThingsSubjectLongTermRadon;}
char*   get_mqttTopicAirThingsSubjectShortTermRadon() {return (char*)configDataBase.mqttTopicAirThingsSubjectShortTermRadon;}
char*   get_mqttTopicAirThingsSubjectPressure() {return (char*)configDataBase.mqttTopicAirThingsSubjectPressure;}
char*   get_mqttTopicAirThingsSubjectVoc() {return (char*)configDataBase.mqttTopicAirThingsSubjectVoc;}
char*   get_mqttTopicAirThingsSubjectTimeDate() {return (char*)configDataBase.mqttTopicAirThingsSubjectTimeDate;}

char*   get_mqttTopicRtc()       {return (char*)configDataBase.mqttTopicRtc;}
char*   get_mqttTopicConfig()    {return (char*)configDataBase.mqttTopicConfig;}

bool    get_rtcUseAlarm()       {return (bool)configDataBase.rtcUseAlarm;}
bool    get_rtcUseSquareWave()  {return (bool)configDataBase.rtcUseSquareWave;}
uint8_t get_rtcInterruptPin()   {return (uint8_t)configDataBase.rtcInterruptPin;}

// Submodule enablers
bool    get_enable_AirThingsWavePlus() {return enableFlags.enable_AirThingsWavePlus;}
bool    get_enable_rtcPlus()           {return enableFlags.enable_rtcPlus;}
bool    get_enable_cpuTemperature()    {return enableFlags.enable_cpuTemperature;}
bool    get_enable_wiFiPlus()          {return enableFlags.enable_wiFiPlus;}
bool    get_enable_mqtt()              {return enableFlags.enable_mqtt;}
bool    get_enable_webserver()         {return enableFlags.enable_webserver;}
bool    get_enable_eepromAT24C32()     {return enableFlags.enable_eepromAT24C32;}
bool    get_enable_lcdDisplay()        {return enableFlags.enable_eepromAT24C32;}


// From config file
uint8_t  _get_configRevision() {return (uint8_t)config::revision;}

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
int16_t _get_mqttPort()         {return (int16_t)config::mqttPort;}
char*   _get_mqttTopicAirThings() {return (char*)config::mqttTopicAirThings;}
char*   _get_mqttTopicAirThingsSubjectTemperature() {return (char*)config::mqttTopicAirThingsSubjectTemperature;}
char*   _get_mqttTopicAirThingsSubjectHumidity() {return (char*)config::mqttTopicAirThingsSubjectHumidity;}
char*   _get_mqttTopicAirThingsSubjectCO2() {return (char*)config::mqttTopicAirThingsSubjectCO2;}
char*   _get_mqttTopicAirThingsSubjectLongTermRadon() {return (char*)config::mqttTopicAirThingsSubjectLongTermRadon;}
char*   _get_mqttTopicAirThingsSubjectShortTermRadon() {return (char*)config::mqttTopicAirThingsSubjectShortTermRadon;}
char*   _get_mqttTopicAirThingsSubjectPressure() {return (char*)config::mqttTopicAirThingsSubjectPressure;}
char*   _get_mqttTopicAirThingsSubjectVoc() {return (char*)config::mqttTopicAirThingsSubjectVoc;}
char*   _get_mqttTopicAirThingsSubjectTimeDate() {return (char*)config::mqttTopicAirThingsSubjectTimeDate;}

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

    configDataBase.revision         = config::revision;
    configDataBase.wiFiUseStaticIP  = config::wiFiUseStaticIP;
    configDataBase.mqttPort         = config::mqttPort;
    configDataBase.rtcUseAlarm      = config::rtcUseAlarm;
    configDataBase.rtcUseSquareWave = config::rtcUseSquareWave;
    configDataBase.rtcInterruptPin  = config::rtcInterruptPin;

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

    strncpy(configDataBase.mqttTopicAirThingsSubjectTemperature, config::mqttTopicAirThingsSubjectTemperature, sizeof(configDataBase.mqttTopicAirThingsSubjectTemperature) - 1);
    configDataBase.mqttTopicAirThingsSubjectTemperature[sizeof(configDataBase.mqttTopicAirThingsSubjectTemperature) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectHumidity, config::mqttTopicAirThingsSubjectHumidity, sizeof(configDataBase.mqttTopicAirThingsSubjectHumidity) - 1);
    configDataBase.mqttTopicAirThingsSubjectHumidity[sizeof(configDataBase.mqttTopicAirThingsSubjectHumidity) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectCO2, config::mqttTopicAirThingsSubjectCO2, sizeof(configDataBase.mqttTopicAirThingsSubjectCO2) - 1);
    configDataBase.mqttTopicAirThingsSubjectCO2[sizeof(configDataBase.mqttTopicAirThingsSubjectCO2) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectLongTermRadon, config::mqttTopicAirThingsSubjectLongTermRadon, sizeof(configDataBase.mqttTopicAirThingsSubjectLongTermRadon) - 1);
    configDataBase.mqttTopicAirThingsSubjectLongTermRadon[sizeof(configDataBase.mqttTopicAirThingsSubjectLongTermRadon) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectShortTermRadon, config::mqttTopicAirThingsSubjectShortTermRadon, sizeof(configDataBase.mqttTopicAirThingsSubjectShortTermRadon) - 1);
    configDataBase.mqttTopicAirThingsSubjectShortTermRadon[sizeof(configDataBase.mqttTopicAirThingsSubjectShortTermRadon) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectPressure, config::mqttTopicAirThingsSubjectPressure, sizeof(configDataBase.mqttTopicAirThingsSubjectPressure) - 1);
    configDataBase.mqttTopicAirThingsSubjectPressure[sizeof(configDataBase.mqttTopicAirThingsSubjectPressure) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectVoc, config::mqttTopicAirThingsSubjectVoc, sizeof(configDataBase.mqttTopicAirThingsSubjectVoc) - 1);
    configDataBase.mqttTopicAirThingsSubjectVoc[sizeof(configDataBase.mqttTopicAirThingsSubjectVoc) - 1] = '\0';

    strncpy(configDataBase.mqttTopicAirThingsSubjectTimeDate, config::mqttTopicAirThingsSubjectTimeDate, sizeof(configDataBase.mqttTopicAirThingsSubjectTimeDate) - 1);
    configDataBase.mqttTopicAirThingsSubjectTimeDate[sizeof(configDataBase.mqttTopicAirThingsSubjectTimeDate) - 1] = '\0';

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

    enableFlags.enable_AirThingsWavePlus =  config::enable_AirThingsWavePlus;
    enableFlags.enable_rtcPlus           =  config::enable_rtcPlus;
    enableFlags.enable_cpuTemperature    =  config::enable_cpuTemperature;
    enableFlags.enable_wiFiPlus          =  config::enable_wiFiPlus;
    enableFlags.enable_mqtt              =  config::enable_mqtt;
    enableFlags.enable_webserver         = config::enable_webserver;
    enableFlags.enable_eepromAT24C32     = config::enable_eepromAT24C32;
    enableFlags.enable_lcdDisplay        = config::enable_lcdDisplay;

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

        uint8_t eepromEnableRegister = api_eeprom::eeprom.getEnableRegister();
        mapEnablegRegisterToEnableStruct(eepromEnableRegister);

        uint8_t eepromConfigRegister = api_eeprom::eeprom.getConfigRegister();
        mapConfigRegisterToConfigStruct(eepromConfigRegister);
        Serial.println("Config database read from EEPROM");
        retValue = true;
    }
    else
    {
        Serial.println("EEPROM not configured, defaulting to config.h");
        retValue = readConfigFromCONFIG_H();
    }

//       /** Write and read a struct */
//   // Declare the struct "Point"
//   struct Point {
//     int x;
//     int y;
//   };
//   Point point = {17, 42};
//   // Write the struct point to the eprom starting at address 0
//   eprom.put(0, point);
//   Point point_in;
//   // Read the struct point_in from eprom starting at address 0
//   eprom.get(0, point_in);
//   Serial.println(point_in.x);
//   Serial.println(point_in.y);

    return retValue;
}

bool writeConfigFromEEPROM()
{
//   /** Write and read a struct */
//   // Declare the struct "Point"
//   struct Point {
//     int x;
//     int y;
//   };
//   Point point = {17, 42};
//   // Write the struct point to the eprom starting at address 0
//   eprom.put(0, point);
//   Point point_in;
//   // Read the struct point_in from eprom starting at address 0
//   eprom.get(0, point_in);
//   Serial.println(point_in.x);
//   Serial.println(point_in.y);
    return true;
}

bool isEepromConfigValid()
{
    uint8_t configRegister = api_eeprom::eeprom.getConfigRegister();
    return ((configRegister & configFlag_loadConfigFromEEPROM) == configFlag_loadConfigFromEEPROM);
}

uint8_t mapEnableStructToEnableRegister()
{
    uint8_t eepromEnableRegister = 0;
    if (enableFlags.enable_AirThingsWavePlus) {eepromEnableRegister |= enableFlag_AirThingsWavePlus;}
    if (enableFlags.enable_rtcPlus)           {eepromEnableRegister |= enableFlag_rtcPlus;}
    if (enableFlags.enable_cpuTemperature)    {eepromEnableRegister |= enableFlag_cpuTemperature;}
    if (enableFlags.enable_wiFiPlus)          {eepromEnableRegister |= enableFlag_wiFiPlus;}
    if (enableFlags.enable_mqtt)              {eepromEnableRegister |= enableFlag_mqtt;}
    if (enableFlags.enable_webserver)         {eepromEnableRegister |= enableFlag_webserver;}
    if (enableFlags.enable_eepromAT24C32)     {eepromEnableRegister |= enableFlag_eepromAT24C32;}
    return eepromEnableRegister;
}

void mapEnablegRegisterToEnableStruct(uint8_t eepromEnableRegister)
{
    memset(&enableFlags, 0x00, sizeof(enableFlags));
    enableFlags.enable_AirThingsWavePlus = ((eepromEnableRegister & enableFlag_AirThingsWavePlus) == enableFlag_AirThingsWavePlus);
    enableFlags.enable_rtcPlus           = ((eepromEnableRegister & enableFlag_rtcPlus) == enableFlag_rtcPlus);
    enableFlags.enable_cpuTemperature    = ((eepromEnableRegister & enableFlag_cpuTemperature) == enableFlag_cpuTemperature);
    enableFlags.enable_wiFiPlus          = ((eepromEnableRegister & enableFlag_wiFiPlus) == enableFlag_wiFiPlus);
    enableFlags.enable_mqtt              = ((eepromEnableRegister & enableFlag_mqtt) == enableFlag_mqtt);
    enableFlags.enable_webserver         = ((eepromEnableRegister & enableFlag_webserver) == enableFlag_webserver);
    enableFlags.enable_eepromAT24C32     = ((eepromEnableRegister & enableFlag_eepromAT24C32) == enableFlag_eepromAT24C32);
}

uint8_t mapConfigStructToConfigRegister()
{
    uint8_t eepromConfigRegister = 0;
    if (configFlags.config_loadConfigFromEEPROM) {eepromConfigRegister |= enableFlag_AirThingsWavePlus;}
    return eepromConfigRegister;
}

 void mapConfigRegisterToConfigStruct(uint8_t eepromConfigRegister)
{
    memset(&configFlags, 0x00, sizeof(configFlags));
    configFlags.config_loadConfigFromEEPROM = ((eepromConfigRegister & configFlag_loadConfigFromEEPROM) == configFlag_loadConfigFromEEPROM);
}

} // namespace api_config
