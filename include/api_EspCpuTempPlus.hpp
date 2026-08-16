#ifndef ESP_CPU_TEMP_H
#define ESP_CPU_TEMP_H

#include <Arduino.h>

#include <driver/temperature_sensor.h>
#define NEW_DRIVER_AVAILABLE

namespace api_EspCpuTempPlus
{

class ESPCPUTemp {
public:
    ESPCPUTemp();
    ~ESPCPUTemp();

    bool begin();

    bool tempAvailable() const;

    float getTemp();

private:
    String chip_model;
    bool sensor_available;

#ifdef NEW_DRIVER_AVAILABLE
    temperature_sensor_handle_t temp_sensor_handle;
#endif

    bool initTempSensorLegacy();

    bool initTempSensorNew();

    bool readTempLegacy(float &result);

    bool readTempNew(float &result);
};

extern ESPCPUTemp cpuTemperature;

} // namespace api_EspCpuTempPlus
#endif // ESP_CPU_TEMP_H
