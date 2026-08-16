
#ifndef API_AIRTHINGSWAVEPLUE_HPP
#define API_AIRTHINGSWAVEPLUE_HPP

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

namespace api_AirThingsWavePlus {

struct CurrentValues
{
  #define HUMIDIY_SCALING     (2.0f)
  #define TEMPERATURE_SCALING (9.0f/(5.0f*100.0f))+32.0f
  #define RADON_SCALING       (37.0f)
  #define PRESSURE_SCALING    (50.0f)

  float humidity;
  float radonSTA;
  float radonLTA;
  float temperature;
  float pressure;
  float co2;
  float voc;

  static CurrentValues fromBytes(uint8_t *data);
};

class Wave2
{
private:
  uint16_t id;
  uint32_t serialNumber;
  String   macAddress;

  struct DeviceValues
  {
    uint16_t id;
    uint32_t serialNumber;
  };

public:
  Wave2(uint32_t serial);

  void   setup(uint32_t serial);
  bool   isConnected();
  String discover();
  bool   connect(int retries = 1);
  void   disconnect();
  CurrentValues read();

private:
  DeviceValues parseDeviceValues(String MFRdata);
};

extern Wave2 wave2;

} // namespace api_AirThingsWavePlus
#endif // API_AIRTHINGSWAVEPLUE_HPP