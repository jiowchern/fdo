//--------------------------------------------------------------------------------------
// Performs timer operations
// Use DXUTGetGlobalTimer() to get the global instance
//--------------------------------------------------------------------------------------

#ifndef CDXUTTimer_H
#define CDXUTTimer_H

#include <Windows.h>

class CDXUTTimer
{
public:
    CDXUTTimer();
    
    void Reset(); // resets the timer
    void Start(); // starts the timer
    void Stop();  // stop (or pause) the timer
    void Advance(); // advance the timer by 0.1 seconds
    double GetAbsoluteTime(); // get the absolute system time
    double GetTime(); // get the current time
    double GetElapsedTime(); // get the time that elapsed between GetElapsedTime() calls
    bool IsStopped(); // returns true if timer stopped
    
protected:
    bool m_bUsingQPF;
    bool m_bTimerStopped;
    LONGLONG m_llQPFTicksPerSec;
    
    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
};
    
CDXUTTimer* DXUTGetGlobalTimer();

#endif