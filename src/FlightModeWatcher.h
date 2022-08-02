#pragma once
#include "Windows.h"

class FlyoutTrigger;

class FlightModeWatcher
{
public:
	static FlightModeWatcher* instance;
	FlyoutTrigger* parentTrigger = NULL;
	HANDLE fmWatcherClosing = NULL;
	FlightModeWatcher();
	~FlightModeWatcher();

	void Start();
	void Stop();
	void OnFlightModeChange();
private:
	int _currentThread = 0;
	HANDLE _watcherThread[2] = {NULL, NULL};
	static DWORD WINAPI ThreadProc(LPVOID);
};

