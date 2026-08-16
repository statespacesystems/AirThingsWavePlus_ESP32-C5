#ifndef API_WIFIPLUS_HPP
#define API_WIFIPLUS_HPP


#include <WiFi.h>
#include <arduino.h>

#include "api_config.hpp"

namespace api_WiFiPlus 
{


class WiFiPlus
{
private:
    bool autoReconnectFlag;
    bool staticIP;

public:
    WiFiPlus(bool useStaticIP);

    void removeWiFiEvent(wifi_event_id_t eventID);
    bool disconnect();
    bool setup();
    bool checkWiFiConnection();
    bool isWiFiConnected();
    void setAutoReconnect(bool flag);

private:
    bool configWiFi();
    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

};

extern WiFiPlus wiFiPlus;

} // namespace api_WiFiPlus
#endif // API_AIRTHINGSWAVEPLUE_HPP