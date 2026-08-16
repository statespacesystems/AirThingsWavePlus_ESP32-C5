/* Example implementation of an alarm using DS3231
 *
 * VCC and GND of RTC should be connected to some power source
 * SDA, SCL of RTC should be connected to SDA, SCL of arduino
 * SQW should be connected to CLOCK_INTERRUPT_PIN
 * CLOCK_INTERRUPT_PIN needs to work with interrupts
 */

#include <RTClib.h>
// #include <Wire.h>
#include <arduino.h>

#include "api_config.hpp"
#include "api_rtc.hpp"

namespace api_Rtc 
{
// the pin that is connected to SQW
// #define CLOCK_INTERRUPT_PIN D2
RtcPlus rtcPlus;

const char* stringDS3231_A1_PerSecond   = "DS3231_A1_PerSecond";
const char* stringDS3231_A1_Second      = "DS3231_A1_Second";
const char* stringDS3231_A1_Minute      = "DS3231_A1_Minute";
const char* stringDS3231_A1_Hour        = "DS3231_A1_Hour";
const char* stringDS3231_A1_Date        = "DS3231_A1_Date";
const char* stringDS3231_A1_Day         = "DS3231_A1_Day";

const char* stringDS3231_A2_PerMinute   = "DS3231_A2_PerMinute";
const char* stringDS3231_A2_Minute      = "DS3231_A2_Minute";
const char* stringDS3231_A2_Hour        = "DS3231_A2_Hour";
const char* stringDS3231_A2_Date        = "DS3231_A2_Date";
const char* stringDS3231_A2_Day         = "DS3231_A2_Day";

const char* stringDS3231_OFF            = "DS3231_OFF";
const char* stringDS3231_SquareWave1Hz  = "stringDS3231_SquareWave1Hz";
const char* stringDS3231_SquareWave1kHz = "DS3231_SquareWave1kHz";
const char* stringDS3231_SquareWave4kHz = "DS3231_SquareWave4kHz";
const char* stringDS3231_SquareWave8kHz = "DS3231_SquareWave8kHz";

const char* warningRtcAlarm             = "WARNING: RTC ALARMS DISABLED -> ENABLE THEM IN CONFIG.CPP";
const char* warningSquareWave           = "WARNING: RTC SQUARE WAVE OUTPUT DISABLED -> ENABLE THEM IN CONFIG.CPP";

const char* emptyString                 = "";

RtcPlus::RtcPlus()
{
    useRtcAlarm                 = api_config::get_rtcUseAlarm();
    useRtcSquareWave            = api_config::get_rtcUseSquareWave();
    rtcInterruptHappened        = false;

    alarmStatus.alarm1Enabled   = false;
    alarmStatus.alarm1Type      = RtcAlarmOneShot;
    alarmStatus.alarm1Mode      = DS3231_A1_Date;
    alarmStatus.alarm1ModeName  = mapAlarm1ModeName(alarmStatus.alarm1Mode);
    alarmStatus.alarm1Time      = (DateTime)(0,0,0);
    alarmStatus.alarm1DeltaTime = TimeSpan(60);
    alarmStatus.alarm1Fired     = false;
    alarmStatus.alarm2Enabled   = false;
    alarmStatus.alarm2Type      = RtcAlarmOneShot;
    alarmStatus.alarm2Mode      = DS3231_A2_Date;
    alarmStatus.alarm2ModeName  = mapAlarm2ModeName(alarmStatus.alarm2Mode);
    alarmStatus.alarm2Time      = (DateTime)(0,0,0);
    alarmStatus.alarm2DeltaTime = TimeSpan(60);
    alarmStatus.alarm2Fired     = false;

    squareWaveStatus.squareWaveMode     = DS3231_OFF;
    squareWaveStatus.squareWaveModeName = mapSquareWaveModeName(squareWaveStatus.squareWaveMode);
}

bool RtcPlus::setup() 
{
    Serial.printf("get_rtcUseAlarm: %d, get_rtcUseSquareWave: %d", api_config::get_rtcUseAlarm(), api_config::get_rtcUseSquareWave());
    delay(10);
    if (    (api_config::get_rtcUseAlarm() == true)
         && (api_config::get_rtcUseSquareWave() == true))
    {
        Serial.println("RTC not installed: Cannot have Alarms and Square Wave Output enabled simultaneously. Update config.cpp");
        Serial.flush();
        return false;
    }

    // initializing the rtc
    if(!RTC_DS3231::begin()) 
    {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        return false;
    }

    if(RtcPlus::lostPower()) 
    {
        // this will adjust to the date and time at compilation
        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    //we don't need the 32K Pin, so disable it
    RTC_DS3231::disable32K();

    // stop oscillating signals at SQW Pin
    // otherwise setAlarm will fail
    RTC_DS3231::writeSqwPinMode(DS3231_OFF);


    // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
    // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
    RtcPlus::clearRtcAlarm1();
    RtcPlus::clearRtcAlarm2();

    // turn off alarms (in case it isn't off already)
    // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
    RtcPlus::disableRtcAlarm1();
    RtcPlus::disableRtcAlarm2();

    if (useRtcAlarm)
    {
        // Making it so, that the alarm will trigger an interrupt
        pinMode(api_config::get_rtcInterruptPin(), INPUT_PULLUP);
        //TODO fix onAlar
        //attachInterrupt(digitalPinToInterrupt(api_config::get_rtcInterruptPin()), onAlarm, FALLING);
    }

    return true;
}

bool  RtcPlus::setRtcTime(const DateTime &dt)
{
    RTC_DS3231::adjust(DateTime(dt));
    return true;
}

void RtcPlus::getRtcTime(char (&timeString)[TIME_STRINGLENGTH])
{
    char date[TIME_STRINGLENGTH] = "hh:mm:ss";
    RTC_DS3231::now().toString(date);
    date[TIME_STRINGLENGTH-1] = '\0';
    memcpy(timeString, date, TIME_STRINGLENGTH);
}

void RtcPlus::getRtcTimeDate(char (&timeDateString)[TIMEDATE_STRINGLENGTH])
{
    char date[TIMEDATE_STRINGLENGTH] = "MM/DD/YY hh:mm:ss";
    RTC_DS3231::now().toString(date);
    date[TIMEDATE_STRINGLENGTH-1] = '\0';
    memcpy(timeDateString, date, TIMEDATE_STRINGLENGTH);
}

bool RtcPlus::setRtcAlarm1(const DateTime &dt, Ds3231Alarm1Mode alarm_mode, RtcAlarmType alarm_type)
{
    if (useRtcAlarm)
    {
        if (RTC_DS3231::setAlarm1(dt, alarm_mode))
        {
            alarmStatus.alarm1DeltaTime = TimeSpan(dt - RTC_DS3231::now());
            alarmStatus.alarm1Mode      = alarm_mode;
            alarmStatus.alarm1ModeName  = mapAlarm1ModeName(alarm_mode);
            alarmStatus.alarm1Type      = alarm_type;
            return true;
        }
        else
        {
            Serial.println("Error, alarm wasn't set!");
            return false;
        }
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool RtcPlus::setRtcAlarm2(const DateTime &dt, Ds3231Alarm2Mode alarm_mode, RtcAlarmType alarm_type)
{
    if (useRtcAlarm)
    {
        if (RTC_DS3231::setAlarm2(dt, alarm_mode))
        {
            alarmStatus.alarm2DeltaTime = TimeSpan(dt - RTC_DS3231::now());
            alarmStatus.alarm2Mode      = alarm_mode;
            alarmStatus.alarm2ModeName  = mapAlarm2ModeName(alarm_mode);
            alarmStatus.alarm2Type      = alarm_type;
            return true;
        }
        else
        {
            Serial.println("Error, alarm wasn't set!");
            return false;
        }
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool  RtcPlus::setRtcAlarm1(const TimeSpan dt, Ds3231Alarm1Mode alarm_mode, RtcAlarmType alarm_type)
{
    if (useRtcAlarm)
    {
        if (RTC_DS3231::setAlarm1(RTC_DS3231::now() + dt, alarm_mode))
        {
            alarmStatus.alarm1DeltaTime = dt;
            alarmStatus.alarm1Mode      = alarm_mode;
            alarmStatus.alarm1ModeName  = mapAlarm1ModeName(alarm_mode);
            alarmStatus.alarm1Type      = alarm_type;
            return true;
        }
        else
        {
            Serial.println("Error, alarm wasn't set!");
            return false;
        }
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool RtcPlus::setRtcAlarm2(const TimeSpan dt, Ds3231Alarm2Mode alarm_mode, RtcAlarmType alarm_type)
{
    if (useRtcAlarm)
    {
        if (RTC_DS3231::setAlarm2(RTC_DS3231::now() + dt, alarm_mode))
        {
            alarmStatus.alarm2DeltaTime = dt;
            alarmStatus.alarm2Mode      = alarm_mode;
            alarmStatus.alarm2ModeName  = mapAlarm2ModeName(alarm_mode);
            alarmStatus.alarm2Type      = alarm_type;
            return true;
        }
        else
        {
            Serial.println("Error, alarm wasn't set!");
            return false;
        }
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

RtcAlarmStatus RtcPlus::getRtcAlarmStatus()
{
    if (!useRtcAlarm)
    {
        Serial.println(warningRtcAlarm);
    }

    return alarmStatus;
}

DateTime  RtcPlus::getRtcAlarm1()
{
    if (useRtcAlarm)
    {
        alarmStatus.alarm1Time = RTC_DS3231::getAlarm1();
    }
    else
    {
        Serial.println(warningRtcAlarm);
        alarmStatus.alarm1Time = (DateTime)(0,0,0);
    }

    return alarmStatus.alarm1Time;
}

DateTime  RtcPlus::getRtcAlarm2()
{
    if (useRtcAlarm)
    {
        alarmStatus.alarm2Time = RTC_DS3231::getAlarm2();
    }
    else
    {
        Serial.println(warningRtcAlarm);
        alarmStatus.alarm2Time = (DateTime)(0,0,0);
    }

    return alarmStatus.alarm2Time;
}

char* RtcPlus::getRtcAlarm1Mode()
{
    if (useRtcAlarm)
    {
        return alarmStatus.alarm1ModeName;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return (char*)emptyString;
    }
}

char* RtcPlus::getRtcAlarm2Mode()
{
    if (useRtcAlarm)
    {
        return alarmStatus.alarm1ModeName;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return (char*)emptyString;
    }
}

bool  RtcPlus::rtcPolling()
{
    if (useRtcAlarm)
    {
        return rtcInterruptHappened;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool  RtcPlus::clearRtcAlarm1()
{
    alarmStatus.alarm1Enabled = false;

    if (useRtcAlarm)
    {
        RTC_DS3231::clearAlarm(1);
        return true;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool  RtcPlus::clearRtcAlarm2()
{
    alarmStatus.alarm2Enabled = false;

    if (useRtcAlarm)
    {
        RTC_DS3231::clearAlarm(2);
        return true;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return false;
    }
}

bool  RtcPlus::disableRtcAlarm1()
{
    alarmStatus.alarm1Enabled = false;
    alarmStatus.alarm1Fired   = false;
    alarmStatus.alarm1Time    = (DateTime)(0,0,0);

    if (useRtcAlarm)
    {
        RTC_DS3231::disableAlarm(1);
        return true;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return 0;
    }
}

bool  RtcPlus::disableRtcAlarm2()
{
    alarmStatus.alarm2Enabled = false;
    alarmStatus.alarm2Fired   = false;
    alarmStatus.alarm2Time    = (DateTime)(0,0,0);

    if (useRtcAlarm)
    {
        RTC_DS3231::disableAlarm(2);
        return true;
    }
    else
    {
        Serial.println(warningRtcAlarm);
        return 0;
    }
}

bool RtcPlus::hasAlarm1Fired()
{
    if (useRtcAlarm)
    {
        return alarmStatus.alarm1Fired;
    }
    else
    {
        Serial.println("WARNING: RTC ALARMS DISABLED -> ENABLE THEM IN CONFIG.CPP");
        return false;
    }
}

bool RtcPlus::hasAlarm2Fired()
{
    if (useRtcAlarm)
    {
        return alarmStatus.alarm2Fired;
    }
    else
    {
        Serial.println("WARNING: RTC ALARMS DISABLED -> ENABLE THEM IN CONFIG.CPP");
        return false;
    }
}

void RtcPlus::onAlarm() 
{
    rtcInterruptHappened = true;
    // Check which alarm fired
    if (RTC_DS3231::alarmFired(1)) 
    {
        clearRtcAlarm1(); // Clear the flag
        alarmStatus.alarm1Fired = true;
        if (alarmStatus.alarm1Type == RtcAlarmRetriggering)
        {
            RTC_DS3231::setAlarm1(RTC_DS3231::now() + alarmStatus.alarm1DeltaTime,
                                  alarmStatus.alarm1Mode);
        }
        // TODO: add callback to a non-rtcPlus function
    }
    if (RTC_DS3231::alarmFired(2)) 
    {
        clearRtcAlarm2(); // Clear the flag
        alarmStatus.alarm2Fired = true;
        if (alarmStatus.alarm2Type == RtcAlarmRetriggering)
        {
            RTC_DS3231::setAlarm2(RTC_DS3231::now() + alarmStatus.alarm2DeltaTime,
                                  alarmStatus.alarm2Mode);
        }
        // TODO: add callback to a non-rtcPlus function
    }
}

char* RtcPlus::readRtcSqwPinMode()
{
    if (useRtcSquareWave)
    {
        return squareWaveStatus.squareWaveModeName;
    }
    else
    {
        Serial.println(warningSquareWave);
        return (char*)stringDS3231_OFF;
    }
}

bool RtcPlus::writeRtcSqwPinMode(Ds3231SqwPinMode mode)
{
    if (useRtcSquareWave)
    {
        RTC_DS3231::writeSqwPinMode(mode);
        return true;
    }
    else
    {
        Serial.println(warningSquareWave);
        return false;
    }
}


char* RtcPlus::mapAlarm1ModeName(Ds3231Alarm1Mode alarmMode)
{
    switch (alarmMode)
    {
        case DS3231_A1_PerSecond: return (char*)stringDS3231_A1_PerSecond;
        case DS3231_A1_Second:    return (char*)stringDS3231_A1_Second;
        case DS3231_A1_Minute:    return (char*)stringDS3231_A1_Minute;
        case DS3231_A1_Hour:      return (char*)stringDS3231_A1_Hour;
        case DS3231_A1_Date:      return (char*)stringDS3231_A1_Date;
        case DS3231_A1_Day:       return (char*)stringDS3231_A1_Day;
        default:                  return (char*)stringDS3231_A1_Date;
    }
}

char* RtcPlus::mapAlarm2ModeName(Ds3231Alarm2Mode alarmMode)
{
    switch (alarmMode)
    {
        case DS3231_A2_PerMinute: return (char*)stringDS3231_A2_PerMinute;
        case DS3231_A2_Minute:    return (char*)stringDS3231_A2_Minute;
        case DS3231_A2_Hour:      return (char*)stringDS3231_A2_Hour;
        case DS3231_A2_Date:      return (char*)stringDS3231_A2_Date;
        case DS3231_A2_Day:       return (char*)stringDS3231_A2_Day;
        default:                  return (char*)stringDS3231_A2_Date;
    }
}

char* RtcPlus::mapSquareWaveModeName(Ds3231SqwPinMode squareWaveMode)
{
    switch (squareWaveMode)
    {
        case DS3231_OFF:            return (char*)stringDS3231_OFF;
        case DS3231_SquareWave1Hz:  return (char*)stringDS3231_SquareWave1Hz;
        case DS3231_SquareWave1kHz: return (char*)stringDS3231_SquareWave1kHz;
        case DS3231_SquareWave4kHz: return (char*)stringDS3231_SquareWave4kHz;
        case DS3231_SquareWave8kHz: return (char*)stringDS3231_SquareWave8kHz;
        default:                    return (char*)stringDS3231_OFF;
    }
}

// void loop() {
//     Serial.print(".");

//     // resetting SQW and alarm 1 flag
//     // using setAlarm1, the next alarm could now be configurated
//     if (rtc.alarmFired(1)) {
//         char date[10] = "hh:mm:ss";
//         rtc.now().toString(date);
//         Serial.print(date);
//         rtc.clearAlarm(1);
//         Serial.print(" - Alarm cleared - ");
//         if (!rtc.setAlarm1(rtc.now() + TimeSpan(30),
//                           DS3231_A1_Minute))
//         {
//             Serial.println("Error, alarm wasn't set!");
//         }
//         else
//         {
//             delay(1);
//             Serial.println("Alarm will happen in 30 seconds!");
//         }

//     }
//     //Serial.println();

//     delay(1000);
// }

// void onAlarm() {
//     Serial.print("\r\nAlarm occured - ");
// }

// /*static uint8_t read_i2c_register(uint8_t addr, uint8_t reg) {
//     Wire.beginTransmission(addr);
//     Wire.write((byte)reg);
//     Wire.endTransmission();

//     Wire.requestFrom(addr, (byte)1);
//     return Wire.read();
// }*/

//     if (rtc.alarmFired(1)) {
//         char date[20] = "MM/DD/YY hh:mm:ss";
//         rtc.now().toString(date);
//         Serial.print(date);
//         rtc.clearAlarm(1);
//         Serial.print(" - Alarm cleared - ");
//         if (!rtc.setAlarm1(rtc.now() + TimeSpan(30),
//                           DS3231_A1_Minute))
}