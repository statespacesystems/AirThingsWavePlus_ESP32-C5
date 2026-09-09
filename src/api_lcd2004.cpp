#include <arduino.h>
#include "api_lcd2004.hpp" 


namespace api_lcd2004
{

//hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

Lcd2004 lcd2004;

bool Lcd2004::lcd_setup()
{
    int status;

	// initialize LCD with number of columns and rows: 
	// hd44780 returns a status from begin() that can be used
	// to determine if initalization failed.
	// the actual status codes are defined in <hd44780.h>
	status = lcd2004.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		Serial.print("LCD initalization failed: ");
		Serial.println(status);

		// blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
        return false;
	}

	// turn on automatic line wrapping
	// which automatically wraps lines to the next lower line and wraps back
	// to the top when at the bottom line
	// NOTE: 
	// noLineWrap() can be used to disable automatic line wrapping.
	// _write() can be called instead of write() to send data bytes
	// to the display bypassing any special character or line wrap processing.
	lcd2004.lineWrap();

	lcd2004.print("Serial2LCD"); 
	if(LCD_ROWS > 1)
	{
        lcd2004.clear();
		lcd2004.setCursor(0,1);
		lcd2004.print("Passed Init!");
	}

    Serial.println("LCD initialized ");
    return true;
}

bool Lcd2004::lcd_printAirThingsWavePlusData(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu)
{
	lcd2004.setCursor(0,0);
    lcd2004.print(timeString);
    char lineString[20];
	lcd2004.setCursor(0,1);
    memset(lineString, 0x20, sizeof(lineString));
    snprintf(lineString,sizeof(lineString), "tmp %3.2f  h %3.2f", currentValues.temperature, currentValues.humidity);
    lcd2004.print(lineString);
	lcd2004.setCursor(0,2);
    memset(lineString, 0x20, sizeof(lineString));
    snprintf(lineString,sizeof(lineString), "CO2 %4.2f v %3.2f", currentValues.co2, currentValues.voc);
    lcd2004.print(lineString);
	lcd2004.setCursor(0,3);
    memset(lineString, 0x20, sizeof(lineString));
    snprintf(lineString,sizeof(lineString), "RadS %2.2f RadL %2.2f", currentValues.radonSTA, currentValues.radonLTA);
    lcd2004.print(lineString);
    return true;

}


} // namespace api_lcd2004
