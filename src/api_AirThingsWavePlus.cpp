
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

#include "api_AirThingsWavePlus.hpp"
#include "api_config.hpp"

namespace api_AirThingsWavePlus {

static BLEClient *pClient = nullptr;
static BLERemoteCharacteristic *pCharacteristic = nullptr;
Wave2 wave2((uint32_t) 0);

CurrentValues CurrentValues::fromBytes(uint8_t *data)
{
  uint8_t version = data[0];

  if (version == 1)
  {
    uint8_t humidity     = data[1];
    uint16_t radonSTA    = data[5] << 8 | data[4];
    uint16_t radonLTA    = data[7] << 8 | data[6];
    uint16_t temperature = data[9] << 8 | data[8];
    uint16_t pressure    = data[11] << 8 | data[10];
    uint16_t co2         = data[13] << 8 | data[12];
    uint16_t voc         = data[15] << 8 | data[14];

     return {
        (float)humidity / HUMIDIY_SCALING,
        (float)radonSTA / RADON_SCALING,
        (float)radonLTA / RADON_SCALING,
        (float)temperature * TEMPERATURE_SCALING,
        (float)pressure / PRESSURE_SCALING,
        (float)co2,
        (float)voc,
    };
  }

  return {};
}


Wave2::Wave2(uint32_t serial) : serialNumber(serial) {}

void Wave2::setup(uint32_t serial)
{
  BLEDevice::init("");
  serialNumber = (uint32_t) serial;
}

bool Wave2::isConnected()
{
  return pClient != nullptr && pClient->isConnected();
}

String Wave2::discover()
{
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  BLEScanResults *foundDevices = (BLEDevice::getScan()->start(3));
  for (int i = 0; i < foundDevices->getCount(); i++)
  {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);

    String mfrData = device.getManufacturerData();
    Wave2::DeviceValues deviceValues = parseDeviceValues(mfrData);
    if (deviceValues.serialNumber == serialNumber)
    {
      serialNumber = deviceValues.serialNumber;
      id = deviceValues.id;
      Serial.println("######################################################################");
      Serial.println("Found device with matching serial number");
      //Serial.printf("ID: %d, SN: %u, %s", id, serialNumber, device.toString());
      Serial.printf("ID: %d, SN: %u, ", id, serialNumber);
      Serial.println(device.toString());
      Serial.println("######################################################################");

      return device.getAddress().toString().c_str();
    }
  }
  return "";
}

bool Wave2::connect(int retries)
{
  int tries = 0;
  while (!isConnected() && tries < retries)
  {
    tries++;
    if (macAddress.length() == 0)
    {
      macAddress = discover();
    }
    if (macAddress.length() != 0)
    {
      if (pClient == nullptr)
      {
         pClient = BLEDevice::createClient();
      }

      bool clientConnectStatus = pClient->connect(BLEAddress(macAddress.c_str()));
      if (!clientConnectStatus)
      {
        //macAddress = "";
        disconnect();
         return false;
      }

      BLERemoteService *pService = pClient->getService(api_config::get_ServiceUUID());
      if (pService != nullptr)
      {
        pCharacteristic = pService->getCharacteristic(api_config::get_CharUUID());
      }
      //delete pService;
    }
  }
  return true;
}

void Wave2::disconnect()
{
  if (pClient != nullptr)
  {
    pClient->disconnect();
    //pClient = nullptr;
    //pCharacteristic = nullptr;
  }
}

CurrentValues Wave2::read()
{
  if (pCharacteristic != nullptr)
  {
    pCharacteristic->readValue();
    uint8_t *data = pCharacteristic->readRawData();
    if (data != nullptr)
    {
      return CurrentValues::fromBytes(data);
    }
  }

  return CurrentValues();
}

Wave2::DeviceValues Wave2::parseDeviceValues(String MFRdata)
{
  uint16_t idLocal           = (MFRdata[1] << 8) | MFRdata[0];
  uint32_t serialNumberLocal = (MFRdata[5] << 24) | (MFRdata[4] << 16) | (MFRdata[3] << 8) | MFRdata[2];

  if (idLocal == 0x0334)
  {
    Serial.printf("\r\nFound AirThings Device: %u\r\n", serialNumberLocal);
    return {idLocal, serialNumberLocal};
  }

  return {0, 0}; // Return 0 for non-Airthings devices
}

} // namespace api_AirThingsWavePlus
