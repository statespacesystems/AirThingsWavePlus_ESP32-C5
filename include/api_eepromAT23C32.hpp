#ifndef API_EEPROM_AT24C32_HPP
#define API_EEPROM_AT24C32_HPP

#include <arduino.h>
#include "at24c32.h"

namespace api_eeprom
{

class EEpromAT24C32 : private AT24C32
{
public:
    EEpromAT24C32();
    bool setupAT24C32();
    uint8_t getEnableRegister();
    uint8_t getConfigRegister();
    uint8_t getAirThingsWavePlusActiveID();
    
    // read/write single byte
    uint8_t readAT24C32( int idx );
    bool writeAT24C32( int idx, uint8_t val);
    bool updateAT24C32( int idx, uint8_t val);

    // read/write buffer
    int writeBufferAT24C32(uint16_t address, const uint8_t* data, size_t len);
    int readBufferAT24C32(uint16_t address, uint8_t* data, size_t len);

    uint16_t lengthAT24C32();
    uint8_t getLastErrorAT24C32();

    template< typename T > T &getAT24C32( int idx, T &t ){
      readBufferAT24C32(idx, (uint8_t*)&t, sizeof(T));
      return t;
    }

    template< typename T > const T &putAT24C32( int idx, const T &t ){
      writeBufferAT24C32(idx, (uint8_t*)&t, sizeof(T));
      return t;
    }

private:
    void printErrorCodeAT24C32(uint8_t errorCode);
};

extern EEpromAT24C32 eeprom;

} // namespace api_eeprom
#endif // API_EEPROM_AT24C32_HPP