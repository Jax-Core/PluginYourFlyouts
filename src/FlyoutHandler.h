#pragma once
#include "DispatchTimer.h"

enum class TriggerType;
class FlyoutTrigger;

class FlyoutHandler
{
public:
	FlyoutHandler();
	~FlyoutHandler();

	static FlyoutHandler* instance;
	static FlyoutTrigger* trigger;
	static std::vector<Measure*> measures;
	HWND HWndHost = NULL;
	HWND HWndDUI = NULL;
	DWORD ShellProcessId = NULL;


	void Initialize(Measure*);
	void Update(int id);
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
	bool _hasFlyoutCreated = false;
	HWINEVENTHOOK HHookID = NULL;
	DispatchTimer* _timer = NULL;

	// hooks shell window events
	void Hook();
	void Unhook();
	void Rehook();
	void TryRehook();

	static void CALLBACK TimerProc(PVOID, BOOLEAN);

	// windows event procedure
	// looks for flyout window events
	static void CALLBACK WinEventProc(
		HWINEVENTHOOK hWinEventHook,
		DWORD event,
		HWND hwnd,
		LONG idObject,
		LONG idChild,
		DWORD idEventThread,
		DWORD dwmsEventTime
	);

	// handle flyout window events
	void OnFlyoutShown();
	void OnFlyoutDestroyed();
	void OnFlyoutHidden();

	void OnChangeAction();

	// helper functions
	static DWORD GetShellProcessId();
	static bool IsShellProcess();
	bool GetAllInfos();
};

