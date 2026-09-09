#include <Arduino.h>
#include "api_AirThingsWavePlus.hpp"
#include "api_WiFiPlus.hpp"
#include "api_rtc.hpp"
#include "api_EspCpuTempPlus.hpp"
#include "api_eepromAT23C32.hpp"
#include "api_mqtt.hpp"
#include "api_lcd2004.hpp"

//namespace std {

#ifdef LOG_LOCAL_LEVEL
#undef LOG_LOCAL_LEVEL
#endif
#define LOG_LOCAL_LEVEL ESP_LOG_MAX //ESP_LOG_NONE

const int SAMPLE_PERIOD = 60; // Time in seconds between reading the current values
static uint32_t msecMax = 0;
static char timeString[TIME_STRINGLENGTH];
static char timeDateString[TIMEDATE_STRINGLENGTH];

static bool enable_AirThingsWavePlus = true;
static bool enable_rtcPlus           = true;
static bool enable_cpuTemperature    = true;
static bool enable_wiFiPlus          = true;
static bool enable_mqtt              = true;
static bool enable_webserver         = false;
static bool enable_eepromAT24C32     = true;
static bool enable_lcdDisplay        = false;

static float temperatureEsp32Cpu = 0.0;

void printAirThingsWavePlus(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu);

void setup()
{
  Serial.begin(115200);

  api_config::readConfig();
  if (enable_AirThingsWavePlus)
  {
    api_AirThingsWavePlus::wave2.setup(api_config::get_AirThingsWavePlusSerialNumber());
  }

  if (enable_wiFiPlus)
  {
    if (!(api_WiFiPlus::wiFiPlus.setup()))
    {
      Serial.println("FAILED to setup WiFi");
      Serial.flush();
      enable_wiFiPlus = false;
    }
  }

  if (enable_rtcPlus)
  {
    if (!(api_Rtc::rtcPlus.setup()))
    {
      Serial.println("FAILED to setup RTC");
      Serial.flush();
      enable_rtcPlus = false;
    }
  }

  if (enable_cpuTemperature)
  {
    if (api_EspCpuTempPlus::cpuTemperature.begin()) 
    {
      Serial.println("Temperature sensor initialized successfully");
    } 
    else 
    {
      Serial.println("Failed to initialize temperature sensor");
      enable_cpuTemperature = false;
    }
  }

  if (enable_eepromAT24C32)
  {
    if (api_eeprom::eeprom.setupAT24C32())
    {
      Serial.println("eeprom AT24C32 initialized successfully");
    }
    else
    {
      Serial.println("Failed to initialize eeprom AT24C32");
      enable_eepromAT24C32 = false;
    }
  }

  if (enable_mqtt)
  {
    if( api_mqtt::mqttPubSub.setupMqtt())
    {
      Serial.println("MQTT initialized successfully");
    }
    else
    {
      Serial.println("Failed to initialize MQTT");
      enable_mqtt = false;
    }
  }

  if (enable_lcdDisplay)
  {
  //   if (!(api_lcd2004::lcdDisplay.setup()))
  //   {
  //     Serial.println("FAILED to setup LCD Display");
  //     Serial.flush();
  //     enable_lcdDisplay = false;
  //   }
  }

}

void loop()
{
  msecMax = millis() + SAMPLE_PERIOD * 1000;
  api_AirThingsWavePlus::CurrentValues currentValues;

  if (enable_cpuTemperature)
  {
    temperatureEsp32Cpu = api_EspCpuTempPlus::cpuTemperature.getTemp();

    if (api_EspCpuTempPlus::cpuTemperature.tempAvailable()) 
    {
      if (!isnan(temperatureEsp32Cpu)) 
      {
        temperatureEsp32Cpu = temperatureEsp32Cpu*9/5 + 32;
        // Serial.print("CPU Temperature: ");
        // Serial.print(temperatureEsp32Cpu);
        // Serial.println(" °F");
      } 
      else 
      {
        Serial.println("Failed to read temperature");
        temperatureEsp32Cpu = -99;
      }
    }
  }
  else
  {
    temperatureEsp32Cpu = -99;
  }
    
  if (enable_rtcPlus)
  {
    api_Rtc::rtcPlus.getRtcTimeDate(timeDateString);
  }
  else
  {
    strcpy(timeDateString, "");
  }

  if (enable_AirThingsWavePlus)
  {
    api_AirThingsWavePlus::wave2.connect(5);
    if (api_AirThingsWavePlus::wave2.isConnected())
    {
      currentValues = api_AirThingsWavePlus::wave2.read();
      printAirThingsWavePlus(timeDateString, currentValues, temperatureEsp32Cpu);
    }
    else
    {
      Serial.println("WARNING: NO AirThingsWavePlus DATA");
    }

    api_AirThingsWavePlus::wave2.disconnect();
  }
  else
  {
    Serial.println("WARNING: NO AirThingsWavePlus DATA");
  }

  if (enable_mqtt)
  {
    api_mqtt::mqttPubSub.updateMqtt(timeDateString, currentValues, temperatureEsp32Cpu);
  }

  if (enable_webserver)
  {

  }

  if (enable_eepromAT24C32)
  {

  }

while(millis() < msecMax);

}

void printAirThingsWavePlus(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu)
{
    Serial.printf("%s, Temperature: %5.2fF, Humidity: %5.2f%%rH, Radon STA: %5.2fpCi/L, Radon LTA:"
                  " %5.2fpCi/L, Pressure: %6.2fmBar, CO2: %5.2fppm, VoC: %5.2fppm, CPU Temp: %5.2fF\r\n", 
                  timeString,
                  currentValues.temperature,
                  currentValues.humidity,
                  currentValues.radonSTA,
                  currentValues.radonLTA,
                  currentValues.pressure,
                  currentValues.co2,
                  currentValues.voc,
                  temperatureEsp32Cpu);
    }

//}