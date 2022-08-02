#include "DispatchTimer.h"

DispatchTimer::DispatchTimer(DWORD ms, WAITORTIMERCALLBACK timerProc, PVOID paramter) :
    timerRunning(false),
    _ms(ms),
    _parameter(paramter),
    _timerProc(timerProc),
    _hQueue(NULL),
    _hTimer(NULL)
{
}

DispatchTimer::~DispatchTimer()
{
    if (timerRunning)
        Stop();
}

void DispatchTimer::Start()
{
    if (timerRunning) return;
    if ((_hQueue = CreateTimerQueue()) == NULL) {
        return;
    }
    if (!CreateTimerQueueTimer(
        &_hTimer,
        _hQueue,
        _timerProc,
        _parameter,
        _ms, _ms,
        WT_EXECUTEDEFAULT
    )) {
        return;
    }
    timerRunning = true;
}

void DispatchTimer::Stop()
{
    if (!timerRunning) return;
    if (!DeleteTimerQueueEx(_hQueue, INVALID_HANDLE_VALUE)) {
        return;
    }
    _hQueue = NULL;
    _hTimer = NULL;
    timerRunning = false;
}