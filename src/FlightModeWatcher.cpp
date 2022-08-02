#include "YourFlyouts.h"
#include "FlightModeWatcher.h"
#include "FlyoutTrigger.h"
#include "FlyoutHandler.h"
#include "shlwapi.h"

FlightModeWatcher* FlightModeWatcher::instance;

FlightModeWatcher::FlightModeWatcher()
{
    fmWatcherClosing = CreateEvent(NULL, true, false, NULL);
}

FlightModeWatcher::~FlightModeWatcher()
{
    Stop();
}

void FlightModeWatcher::Start()
{
    _watcherThread[_currentThread] = CreateThread(
        NULL,
        0,
        ThreadProc,
        instance,
        0,
        NULL
    );
    _currentThread = 1 - _currentThread;
}

void FlightModeWatcher::Stop()
{
    SetEvent(fmWatcherClosing);
    WaitForSingleObject(_watcherThread[1 - _currentThread], INFINITE);
    if (_watcherThread[0] != NULL)
        CloseHandle(_watcherThread[0]);
    if (_watcherThread[1] != NULL)
        CloseHandle(_watcherThread[1]);
    CloseHandle(fmWatcherClosing);
}

DWORD __stdcall FlightModeWatcher::ThreadProc(LPVOID lpParam)
{
    FlightModeWatcher* watcher = (FlightModeWatcher*)lpParam;
    if (watcher == NULL)
        return 0;

    watcher->parentTrigger->parentHandler->Log(LOG_NOTICE, L"inside thread!");

    HANDLE watched[2] = {NULL, NULL};

    // make fmWathcerClosing the first event since
    // WaitForMultipleObjects() returns the smallest
    // index if two events were signalled at the same time
    watched[0] = watcher->fmWatcherClosing;
    while ((watched[1] = CreateEvent(NULL, true, false, NULL)) == NULL);

    HUSKEY fmRegKey;
    if (SHRegOpenUSKey(
        L"SYSTEM\\CurrentControlSet\\Control\\RadioManagement\\SystemRadioState",
        KEY_READ,
        NULL,
        &fmRegKey,
        true
    ) != ERROR_SUCCESS) {
        return 1;
    }
    RegNotifyChangeKeyValue((HKEY)fmRegKey, false, REG_NOTIFY_CHANGE_LAST_SET, watched[1], true);
    
    DWORD waitResult = WaitForMultipleObjects(2, watched, false, INFINITE);
    watcher->parentTrigger->parentHandler->Log(LOG_NOTICE, L"wait finished!");

    if (waitResult >= 0 && waitResult < 2)
    {
        switch (waitResult)
        {
        case 1:
            if (watcher == NULL)
                return 0;
            watcher->parentTrigger->parentHandler->Log(LOG_NOTICE, L"flight mode changed!");
            watcher->OnFlightModeChange();
            CloseHandle(watched[1]);
            break;
        case 0:
            return 0;
        default:
            break;
        }
    }

    return 0;
}
