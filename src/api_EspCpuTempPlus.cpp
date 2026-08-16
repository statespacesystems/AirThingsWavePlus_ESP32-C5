#include "api_EspCpuTempPlus.hpp"

namespace api_EspCpuTempPlus
{

ESPCPUTemp cpuTemperature;

ESPCPUTemp::ESPCPUTemp() : sensor_available(false) {
    temp_sensor_handle = NULL;
}

ESPCPUTemp::~ESPCPUTemp() {
    if (temp_sensor_handle != NULL) {
        temperature_sensor_disable(temp_sensor_handle);
        temperature_sensor_uninstall(temp_sensor_handle);
        temp_sensor_handle = NULL;
    }
}

bool ESPCPUTemp::initTempSensorLegacy() {
    Serial.println("Legacy driver not available in this Arduino core");
    return false;
}

bool ESPCPUTemp::initTempSensorNew() {
    temperature_sensor_config_t temp_sensor = {
        .range_min = -10, // -10°C
        .range_max = 80,  // 80°C
    };
    esp_err_t ret = temperature_sensor_install(&temp_sensor, &temp_sensor_handle);
    if (ret != ESP_OK) {
        Serial.printf("Failed to install new temperature sensor (err: %d)\n", ret);
        return false;
    }
    ret = temperature_sensor_enable(temp_sensor_handle);
    if (ret != ESP_OK) {
        Serial.printf("Failed to enable new temperature sensor (err: %d)\n", ret);
        return false;
    }
    Serial.println("New temperature sensor initialized");
    return true;
}

bool ESPCPUTemp::begin() {
    chip_model = ESP.getChipModel();
    Serial.print("Detected chip model: ");
    Serial.println(chip_model);

    sensor_available = false;

    sensor_available = initTempSensorNew();

    if (!sensor_available) {
        Serial.println("ESP CPU temperature sensor initialization failed");
    }
    return sensor_available;
}

bool ESPCPUTemp::tempAvailable() const {
    return sensor_available;
}

bool ESPCPUTemp::readTempLegacy(float &result) {
    Serial.println("Legacy driver not available");
    return false;
}

bool ESPCPUTemp::readTempNew(float &result) {
    esp_err_t ret = temperature_sensor_get_celsius(temp_sensor_handle, &result);
    if (ret != ESP_OK) {
        Serial.printf("Failed to read temperature (new, err: %d)\n", ret);
        return false;
    }
    return true;
}

float ESPCPUTemp::getTemp() {
    if (!sensor_available) {
        Serial.println("Temperature sensor not available");
        return NAN;
    }

    float result = 0;
    bool success = false;

    success = readTempNew(result);

    return success ? result : NAN;
}
} // namespace api_EspCpuTempPlus