#include <WiFi.h>
#include <arduino.h>

#include "api_config.hpp"
#include "api_WiFiPlus.hpp"

namespace api_WiFiPlus
{

//WiFiPlus wiFiPlus((bool)api_config::get_useStaticIP());
WiFiPlus wiFiPlus((bool)true);
//WiFiPlus wiFiPlus((bool)false);

WiFiPlus::WiFiPlus(bool useStaticIP) : staticIP(useStaticIP) 
{
    autoReconnectFlag = true;
}

    void WiFiPlus::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("\r\n[WiFi] Callback: Connected to AP successfully!");
   }

    void WiFiPlus::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.printf("\r\n[WiFi] Callback: Connected! IP: %s\r\n", WiFi.localIP().toString().c_str());
    }

    void WiFiPlus::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.printf("\r\n[WiFi] Callback: lost connection. Reason: %d\r\n", info.wifi_sta_disconnected.reason);
            // Configures static IP address
            disconnect();
            delay(1000);
            if (!configWiFi())
            {
                Serial.println("STA Failed to configure");
            }
            Serial.println("Trying to Reconnect");
            WiFi.reconnect();
        }

    bool WiFiPlus::configWiFi()
    {
        uint8_t (&LocalIp)[4]      = api_config::get_LocalIp();
        uint8_t (&Gateway)[4]      = api_config::get_Gateway();
        uint8_t (&Subnet)[4]       = api_config::get_Subnet();
        uint8_t (&PrimaryDNS)[4]   = api_config::get_PrimaryDNS();
        uint8_t (&SecondaryDNS)[4] = api_config::get_SecondaryDNS();

        IPAddress local_IP(LocalIp[0], LocalIp[1], LocalIp[2], LocalIp[3]);
        IPAddress gateway(Gateway[0], Gateway[1], Gateway[2], Gateway[3]);
        IPAddress subnet(Subnet[0], Subnet[1], Subnet[2], Subnet[3]);
        IPAddress primaryDNS(PrimaryDNS[0], PrimaryDNS[1], PrimaryDNS[2], PrimaryDNS[3]); // optional
        IPAddress secondaryDNS(SecondaryDNS[0], SecondaryDNS[1], SecondaryDNS[2], SecondaryDNS[3]);
        return WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
   }

    void WiFiPlus::removeWiFiEvent(wifi_event_id_t eventID)
    {
        Serial.printf("WiFi Event ID: %u", eventID);
        WiFi.removeEvent(eventID);
    }

    void WiFiPlus::setAutoReconnect(bool flag)
    {
        Serial.printf("WiFi autoReconnect: %u", flag);
        WiFi.setAutoReconnect(flag);
        autoReconnectFlag = flag;
    }

    bool WiFiPlus::disconnect()
    {
        bool retval = false;
        if (WiFi.disconnect(true, false)) // true=turn off radio, false=keep credentials
        {
            retval = true;
            Serial.println("[WiFi] Disconnected!");
        }

        delay(1000);
        return retval;
    }

    bool WiFiPlus::setup()
    {
        if (staticIP)
        {
            // Configures static IP address
            if (!configWiFi())
            {
                Serial.println("STA Failed to configure");
                return false;
            }

            // delete old config
            //WiFi.disconnect(true);

            delay(1000);
        }

        // Captures 'this' and forwards arguments
        auto callbackWiFiStationConnected = [this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiStationConnected(event, info); };
        auto callbackWiFiGotIP = [this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiGotIP(event, info); };
        auto callbackWiFiStationDisconnected = [this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiStationDisconnected(event, info); };


        WiFi.onEvent( callbackWiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent( callbackWiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent( callbackWiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        Serial.println();
        Serial.println();
        Serial.print("[WiFi] Waiting for WiFi ...");

        int16_t loopCounter = 60;

        // Connect to WiFi
        WiFi.begin(api_config::get_ssid(), api_config::get_password());
        while (!isWiFiConnected() && (loopCounter-- > 0))
        {
            delay(500);
            Serial.print(".");
        }

        if (isWiFiConnected())
        {
            Serial.printf("\r\n[WiFi] Connected! IP: %s\r\n", WiFi.localIP().toString().c_str());
            Serial.printf("[WiFi] RRSI: %d\r\n", WiFi.RSSI());
            return true;
        }
        else
        {
            Serial.println("\r\n[WiFi] Failed to connect!");
            return false;
        }
    }

    bool WiFiPlus::checkWiFiConnection()
    {
        bool retVal = false;
        const uint32_t interval = 60000; // 60 seconds

        if (!autoReconnectFlag)
        {
            uint32_t currentMillis = millis();
            static uint32_t previousMillis = 0;

            // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
            if (!isWiFiConnected() && (currentMillis - previousMillis >= interval))
            {
                Serial.print(millis());
                Serial.println("Reconnecting to WiFi...");
                WiFi.disconnect();
                WiFi.reconnect();
                previousMillis = currentMillis;
                retVal = true;
            }
        }
        else
        {
            Serial.println("ERROR: Cannot exxecute reconnect when autoReconnect is enabled");
        }

        return retVal;
    }

    bool WiFiPlus::isWiFiConnected()
    {
        return (WiFi.status() == WL_CONNECTED);
    }



} // namespace api_WiFiPlus