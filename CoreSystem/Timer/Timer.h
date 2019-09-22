#ifndef TIMER_H
#define TIMER_H
//----------------------------------------------------------------------------
#include "singleton.h"

#define g_Timer Timer::GetSingleton()

namespace FDO
{

    class Timer : public Singleton<Timer>
    {
    private:
        unsigned int m_nLastTime;
        unsigned int m_nTimeDelta;
        int m_iServerTime;
    public:
         Timer(): m_iServerTime(0) {}
        ~Timer() {}

        void InitLastTime( const unsigned int &currTime ) { m_nLastTime=currTime; }
        void CalculateTimeDelta(const unsigned int &currTime)
        {
            m_nTimeDelta = currTime - m_nLastTime;
            m_nLastTime = currTime;
        }

        unsigned int GetTimeDelta() { return m_nTimeDelta; }
        unsigned int GetTimeDelta4X() { return m_nTimeDelta*4; }
        unsigned int GetLastTime() { return m_nLastTime; }

        //void SetServertime( int iServerTime) { m_iServerTime=iServerTime; }
        //int GetServerTime() { return m_iServerTime;}

        float GetfLastTime() { return m_nLastTime*0.001f; }
        float GetfTimeDelta() { return m_nTimeDelta*0.001f; }
    };
}


//----------------------------------------------------------------------------
#endif // TIMER_H