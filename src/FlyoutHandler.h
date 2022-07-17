#pragma once
#include "YourFlyouts.h"
#include "psapi.h" // GetModuleBaseName()
class FlyoutTrigger;

class FlyoutHandler
{
public:
	FlyoutHandler();
	~FlyoutHandler();

	static FlyoutHandler* instance;
	static FlyoutTrigger* trigger;
	static std::vector<Measure*> measures;


	void Initialize(Measure*);
	void Update(int id); // planning to use default skin updates instead of timers
	void Finalize();
	void AddMeasure(Measure*);
	void RemoveMeasure(Measure*);
	void HideNativeFlyout();
	void PermanentlyHideNativeFlyout();
	void ShowNativeFlyout();
	void VerifyNativeFlyoutCreated();

	// helper functions
	static void Log(int, LPWSTR);
private:
	HWND HWndHost = NULL;
	HWND HWndDUI = NULL;
	DWORD ShellProcessId = NULL;
	bool _hasFlyoutCreated = false;
	HWINEVENTHOOK HHookID = NULL;
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;
	UINT_PTR _timerHandle = NULL;
	bool _isTimerRunning = false;

	void Hook();
	void Unhook();
	void Rehook();
	void TryRehook();
	void SetTimer();
	void KillTimer();
	static void CALLBACK TimerProc(PVOID, BOOLEAN);
	static void CALLBACK WinEventProc(
		HWINEVENTHOOK hWinEventHook,
		DWORD event,
		HWND hwnd,
		LONG idObject,
		LONG idChild,
		DWORD idEventThread,
		DWORD dwmsEventTime
	);
	void OnFlyoutShown();
	void OnFlyoutDestroyed();
	void OnFlyoutHidden();
	static DWORD GetShellProcessId();
	static bool IsShellProcess();
	bool GetAllInfos();
};

