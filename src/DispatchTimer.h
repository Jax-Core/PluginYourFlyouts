#pragma once
#include <Windows.h>

class DispatchTimer
{
public:
    DispatchTimer(DWORD ms, WAITORTIMERCALLBACK timerProc, PVOID paramter);
    ~DispatchTimer();
    void Start();
    void Stop();

    bool timerRunning;
private:
    DWORD _ms;
    PVOID _parameter;
    WAITORTIMERCALLBACK _timerProc;
    HANDLE _hQueue;
    HANDLE _hTimer;
};
