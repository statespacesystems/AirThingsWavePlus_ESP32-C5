#include <arduino.h>
#include "api_eepromAT23C32.hpp" 

namespace api_eeprom
{

const char* stringErrorCode0 = "AT24C32 EEPROM not enabled";
const char* stringErrorCode1 = "length to long for buffer";
const char* stringErrorCode2 = "address send, NACK received - typically means no device at the address";
const char* stringErrorCode3 = "data send, NACK received";
const char* stringErrorCode4 = "other twi error (lost bus arbitration, bus error, ..)";

const uint16_t eepromMap_activationStatus               = 0x00;
const uint16_t eepromMap_configRegister                  = 0x01;
const uint16_t eepromMap_enableRegister                  = 0x02;
const uint16_t eepromMap_configAirThingsWavePlusActiveID = 0x03;
const uint16_t eepromMap_TBD_0x04                        = 0x04;
const uint16_t eepromMap_TBD_0x05                        = 0x05;
const uint16_t eepromMap_TBD_0x06                        = 0x06;
const uint16_t eepromMap_TBD_0x74                        = 0x7;
const uint16_t eepromMap_TBD_0x08                        = 0x08;
const uint16_t eepromMap_TBD_0x09                        = 0x09;
const uint16_t eepromMap_TBD_0x0A                        = 0x0A;
const uint16_t eepromMap_TBD_0x0B                        = 0x0B;
const uint16_t eepromMap_TBD_0x0C                        = 0x0C;
const uint16_t eepromMap_TBD_0x0D                        = 0x0D;
const uint16_t eepromMap_TBD_0x0E                        = 0x0E;
const uint16_t eepromMap_TBD_0x0F                        = 0x0F;
const uint16_t eepromMap_startOfConfig                   = 0x10;

bool enable_eepromAT24C32     = true;

EEpromAT24C32 eeprom;

// Create a eprom object configured at address 7
EEpromAT24C32::EEpromAT24C32() : AT24C32(AT24C_ADDRESS_7)
{
}

bool EEpromAT24C32::setupAT24C32()
{
    bool retValue = true;

    if (!enable_eepromAT24C32)
    {
        Serial.print("EEpromAT24C32::setupAT24C32 error: ");
        Serial.println(stringErrorCode0);
        //printErrorCodeAT24C32(0);
        retValue = false;
    }
    return retValue;
}

uint8_t EEpromAT24C32::getEnableRegister()
{
    bool errorCode = false;
    uint8_t enableFlag = 0;
    if (   (enable_eepromAT24C32)
        && ((bool)readAT24C32(eepromMap_activationStatus)))
    {
        enableFlag = readAT24C32(eepromMap_enableRegister);
        errorCode = true;
    }

    if (!errorCode)
    {
        Serial.print("EEpromAT24C32::getEnableRegister error: ");
        Serial.println(stringErrorCode0);
        //printErrorCodeAT24C32(0);
    }

    return enableFlag;
}

uint8_t EEpromAT24C32::getConfigRegister()
{
    bool errorCode = false;
    uint8_t configFlag = 0;
    if (   (enable_eepromAT24C32)
        && ((bool)readAT24C32(eepromMap_activationStatus)))
    {
        configFlag = readAT24C32(eepromMap_configRegister);
        errorCode = true;
    }

    if (!errorCode)
    {
        Serial.print("EEpromAT24C32::getEnableRegister error: ");
        Serial.println(stringErrorCode0);
        //printErrorCodeAT24C32(0);
    }

    return configFlag;
}

uint8_t EEpromAT24C32::getAirThingsWavePlusActiveID()
{
    bool validOperation = false;
    uint8_t retValue    = 0;
    if (   (enable_eepromAT24C32)
        && ((bool)readAT24C32(eepromMap_activationStatus)))
    {
        retValue = (uint8_t)readAT24C32(eepromMap_configAirThingsWavePlusActiveID);
        validOperation = true;
    }
    
    if (!validOperation)
    {
        Serial.print("EEpromAT24C32::getAirThingsWavePlusActiveID error: ");
        Serial.println(stringErrorCode0);
        //printErrorCodeAT24C32(0);
    }

    return retValue;
}

uint8_t EEpromAT24C32::readAT24C32( int idx )
{
    uint8_t retValue = 0;
    retValue = AT24C32::read(idx);
    uint8_t error = AT24C32::getLastError();
    if (error != 0)
    {
        Serial.print("EEpromAT24C32::readAT24C32 error: ");
        //printErrorCodeAT24C32(error);
        Serial.println(error);
    }
    return retValue;
}

bool EEpromAT24C32::writeAT24C32( int idx, uint8_t val)
{
    AT24C32::write(idx, val);
    uint8_t error = AT24C32::getLastError();
    if (error != 0)
    {
        Serial.print("EEpromAT24C32::writeAT24C32 error: ");
        //printErrorCodeAT24C32(error);
        Serial.println(error);
        return false;
    }
    else
    {
        return true;
    }
}

int EEpromAT24C32::writeBufferAT24C32(uint16_t address, const uint8_t* data, size_t len)
{
    int retValue = AT24C32::writeBuffer(address, data, len);
    uint8_t error = AT24C32::getLastError();
    if (error != 0)
    {
        Serial.print("EEpromAT24C32::writeBufferAT24C32 error: ");
        //printErrorCodeAT24C32(error);
        Serial.println(error);
    }
    return retValue;
}

int EEpromAT24C32::readBufferAT24C32(uint16_t address, uint8_t* data, size_t len)
{
    int retValue = AT24C32::readBuffer(address, data, len);
    uint8_t error = AT24C32::getLastError();
    if (error != 0)
    {
        Serial.print("EEpromAT24C32::readBufferAT24C32 error: ");
        //printErrorCodeAT24C32(error);
        Serial.println(error);
    }
    return retValue;

}

bool EEpromAT24C32::updateAT24C32( int idx, uint8_t val)
{
    AT24C32::update(idx, val);
    uint8_t error = AT24C32::getLastError();
    if (error != 0)
    {
        Serial.print("EEpromAT24C32::updateAT24C32 error: ");
        //printErrorCodeAT24C32(error);
        Serial.println(error);
        return false;
    }
    else
    {
        return true;
    }
 }

uint16_t EEpromAT24C32::lengthAT24C32()
{
    return AT24C32::length();
}

void printErrorCodeAT24C32( uint8_t errorCode)
{
    switch(errorCode)
    {
        case 0:
            Serial.println(stringErrorCode0);
            break;
        case 1:
            Serial.println(stringErrorCode1);
            break;
        case 2:
            Serial.println(stringErrorCode2);
            break;
        case 3:
            Serial.println(stringErrorCode3);
            break;
        case 4:
            Serial.println(stringErrorCode4);
            break;
        default:
            Serial.println(stringErrorCode0);
            break;
    }
}

} // namespace api_eeprom