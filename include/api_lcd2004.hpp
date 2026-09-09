#ifndef API_LCD2004_HPP
#define API_LCD2004_HPP

#include <Wire.h>
#include <hd44780.h>						// main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>	// i2c expander i/o class header
#include "api_AirThingsWavePlus.hpp" 

namespace api_lcd2004
{

class Lcd2004 : hd44780_I2Cexp
{
public:
bool lcd_setup();
bool lcd_printAirThingsWavePlusData(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu);

private:
};

extern Lcd2004 lcd2004;

} // namespace api_lcd2004
#endif // API_LCD2004_HPP