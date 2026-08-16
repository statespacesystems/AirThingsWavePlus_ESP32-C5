#ifndef API_RTC_HPP
#define API_RTC_HPP


#include <RTClib.h>
// #include <Wire.h>
#include <arduino.h>

#include "api_config.hpp"

namespace api_Rtc 
{

#define TIME_STRINGLENGTH     10
#define TIMEDATE_STRINGLENGTH 20


typedef enum
{
    RtcAlarmOneShot = 0,
    RtcAlarmRetriggering
} RtcAlarmType;

typedef struct 
{
    bool             alarm1Enabled;
    RtcAlarmType     alarm1Type;
    Ds3231Alarm1Mode alarm1Mode;
    char*            alarm1ModeName;
    DateTime         alarm1Time;
    TimeSpan         alarm1DeltaTime;
    bool             alarm1Fired;
    bool             alarm2Enabled;
    RtcAlarmType     alarm2Type;
    Ds3231Alarm2Mode alarm2Mode;
    char*            alarm2ModeName;
    DateTime         alarm2Time;
    TimeSpan         alarm2DeltaTime;
    bool             alarm2Fired;
} RtcAlarmStatus;


typedef struct 
{  
    Ds3231SqwPinMode squareWaveMode;
    char*            squareWaveModeName;
} RtcSquareWaveStatus;

class RtcPlus : private RTC_DS3231
{
private:
    bool useRtcAlarm;
    bool useRtcSquareWave;
    bool rtcInterruptHappened;
    RtcAlarmStatus alarmStatus;
    RtcSquareWaveStatus squareWaveStatus;

public:
    RtcPlus();

    bool  setup();
    bool  setRtcTime(const DateTime &dt);
    void  getRtcTime(char (&timeString)[TIME_STRINGLENGTH]);
    void  getRtcTimeDate(char (&timeDateString)[TIMEDATE_STRINGLENGTH]);

    bool  setRtcAlarm1(const DateTime &dt, Ds3231Alarm1Mode alarm_mode, RtcAlarmType alarm_type);
    bool  setRtcAlarm2(const DateTime &dt, Ds3231Alarm2Mode alarm_mode, RtcAlarmType alarm_type);
    bool  setRtcAlarm1(const TimeSpan dt, Ds3231Alarm1Mode alarm_mode, RtcAlarmType alarm_type);
    bool  setRtcAlarm2(const TimeSpan dt, Ds3231Alarm2Mode alarm_mode, RtcAlarmType alarm_type);

    DateTime getRtcAlarm1();
    DateTime getRtcAlarm2();
    char* getRtcAlarm1Mode();
    char* getRtcAlarm2Mode();
    bool  clearRtcAlarm1();
    bool  clearRtcAlarm2();
    bool  disableRtcAlarm1();
    bool  disableRtcAlarm2();
    bool  hasAlarm1Fired();
    bool  hasAlarm2Fired();

    RtcAlarmStatus getRtcAlarmStatus();
    bool  rtcPolling();

    char* readRtcSqwPinMode();
    bool  writeRtcSqwPinMode(Ds3231SqwPinMode mode);

private:
    void onAlarm();
    char* mapAlarm1ModeName(Ds3231Alarm1Mode alarmMode);
    char* mapAlarm2ModeName(Ds3231Alarm2Mode alarmMode);
    char* mapSquareWaveModeName(Ds3231SqwPinMode squareWaveMode);
};

extern RtcPlus rtcPlus;

} // namespace api_Rt
#endif // API_RTC_HPP