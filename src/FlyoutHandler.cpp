#include "FlyoutHandler.h"
#include "FlyoutTrigger.h"

std::vector<Measure*> FlyoutHandler::measures;
FlyoutHandler* FlyoutHandler::instance;
FlyoutTrigger* FlyoutHandler::trigger;

FlyoutHandler::FlyoutHandler()
{
}

FlyoutHandler::~FlyoutHandler()
{
	delete trigger;
}

void FlyoutHandler::Initialize(Measure* measure)
{
	instance = this;
	_hasFlyoutCreated = GetAllInfos();
	Hook();
	trigger = new FlyoutTrigger;
	trigger->RegisterShellHook(measure);
	trigger->Hook(measure);
}

void FlyoutHandler::Update(int id)
{
	if (id != NULL && HHookID == NULL)
		TryRehook();
}

void FlyoutHandler::Finalize()
{
	trigger->Unhook();
	Unhook();
	if ((_hasFlyoutCreated = GetAllInfos()))
		ShowNativeFlyout();
}

void FlyoutHandler::AddMeasure(Measure* measure)
{
	if (std::find(measures.begin(), measures.end(), measure) != measures.end())
		return;
	measures.push_back(measure);
	RmLog(measure->rm, LOG_DEBUG, L"Measure added!");
	if (measures.size() == 1) {
		measures[0]->id = 1;
		Initialize(measure);
	}
}

void FlyoutHandler::RemoveMeasure(Measure* measure)
{	
	if (measures.size() == 1)
		Finalize();
	if (std::remove(measures.begin(), measures.end(), measure) != measures.end())
		RmLog(measure->rm, LOG_DEBUG, L"Measure removed!");
	if (measure->id == 1 && !measures.empty())
		measures[0]->id = 1;
}

void FlyoutHandler::Hook()
{
	if (!_hasFlyoutCreated)
		ShellProcessId = GetShellProcessId();

	if (ShellProcessId != NULL)
	{
		HHookID = SetWinEventHook(
			EVENT_OBJECT_CREATE,
			EVENT_OBJECT_STATECHANGE,
			NULL, WinEventProc,
			ShellProcessId, NULL, 
			WINEVENT_OUTOFCONTEXT
		);
		if (HHookID == NULL)
			Log(LOG_ERROR, L"Could not set native flyout hook!");
		else
			Log(LOG_DEBUG, L"Set native flyout hook!");
	}
}

void FlyoutHandler::Unhook()
{
	if (NULL != HHookID)
	{
		UnhookWinEvent(HHookID);
		HHookID = NULL;
		for (auto measure : measures)
		{
			Log(LOG_DEBUG, L"Removed native flyout hook!");
		}
	}
}

void FlyoutHandler::Rehook()
{
	Unhook();
	Hook();
}

void FlyoutHandler::TryRehook()
{
	ShellProcessId = GetShellProcessId();
	if (NULL != ShellProcessId)
	{
		Rehook();
	}
}

void FlyoutHandler::SetTimer()
{
	hTimerQueue = CreateTimerQueue();
	instance->_isTimerRunning = CreateTimerQueueTimer(&instance->hTimer, instance->hTimerQueue, &TimerProc, instance, 3000, 3000, WT_EXECUTEINTIMERTHREAD) != 0;
}

void FlyoutHandler::KillTimer()
{
	if (!_isTimerRunning)
		return;
	while(_isTimerRunning = !DeleteTimerQueueEx(hTimerQueue, INVALID_HANDLE_VALUE));
}

void FlyoutHandler::TimerProc(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	FlyoutHandler* handler = (FlyoutHandler*)lpParam;
	handler->TryRehook();
}

void FlyoutHandler::WinEventProc(
	HWINEVENTHOOK hWinEventHook,
	DWORD eventType,
	HWND hwnd,
	LONG idObject,
	LONG idChild,
	DWORD idEventThread,
	DWORD dwmsEventTime
)
{
	if (idObject != 0 || idChild != 0)
		return;

	if (!instance->_hasFlyoutCreated && (eventType == EVENT_OBJECT_CREATE || eventType == EVENT_OBJECT_SHOW))
	{
		TCHAR className[MAX_CLASS_NAME];
		GetClassName(hwnd, className, MAX_CLASS_NAME);
		if (_wcsicmp(className, L"NativeHWNDHost") == 0)
		{
			instance->_hasFlyoutCreated = instance->GetAllInfos();
			if (instance->_hasFlyoutCreated && hwnd == instance->HWndHost)
			{
				if (eventType == EVENT_OBJECT_SHOW)
				{
					instance->OnFlyoutShown();
				}
			}
		}
	}

	if (hwnd == instance->HWndHost)
	{
		switch (eventType)
		{
		case EVENT_OBJECT_SHOW:
			instance->OnFlyoutShown();
			break;

		case EVENT_OBJECT_DESTROY:
			instance->OnFlyoutDestroyed();
			break;

		case EVENT_OBJECT_HIDE:
			instance->OnFlyoutHidden();
			break;

		default:
			break;
		}
	}
}

void FlyoutHandler::OnFlyoutShown()
{
	Log(LOG_DEBUG, L"Native flyout shown!");
	HideNativeFlyout();
	// PermanentlyHideNativeFlyout();
}

void FlyoutHandler::OnFlyoutDestroyed()
{
	Log(LOG_NOTICE, L"Native flyout destroyed!");
	Unhook();
}

void FlyoutHandler::OnFlyoutHidden()
{
	Log(LOG_DEBUG, L"Native flyout hidden!");
	// ShowNativeFlyout();
}

DWORD FlyoutHandler::GetShellProcessId()
{
	HWND shellWnd = GetShellWindow();
	DWORD pId;
	return GetWindowThreadProcessId(shellWnd, &pId);
}

bool FlyoutHandler::IsShellProcess()
{
	return false;
}

bool FlyoutHandler::GetAllInfos()
{
	HWND hWndHost;
	while ((hWndHost = FindWindowEx(NULL, NULL, L"NativeHWNDHost", L"")) != NULL)
	{
		HWND hWndDUI;
		if ((hWndDUI = FindWindowEx(hWndHost, NULL, L"DirectUIHWND", L"")) != NULL)
		{
			DWORD pId;
			GetWindowThreadProcessId(hWndHost, &pId);
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pId);
			TCHAR moduleBaseName[MAX_PATH];
			if (process == NULL)
			{
				return false;
			}
			GetModuleBaseName(process, NULL, moduleBaseName, MAX_PATH);
			if (_wcsicmp(moduleBaseName, L"explorer.exe") == 0) {
				HWndHost = hWndHost;
				HWndDUI = hWndDUI;
				ShellProcessId = pId;
				return true;
			}
		}
	}
	return false;
}

void FlyoutHandler::Log(int level, LPWSTR msg)
{
	for (auto measure : measures)
	{
		RmLogF(measure->rm, level, L"FlyoutHandler: %s", msg);
	}
}

void FlyoutHandler::HideNativeFlyout()
{
	_hasFlyoutCreated = GetAllInfos();
	if (_hasFlyoutCreated)
	{
		ShowWindowAsync(HWndDUI, SW_MINIMIZE);
		ShowWindowAsync(HWndHost, SW_HIDE);
	}
}

void FlyoutHandler::PermanentlyHideNativeFlyout()
{
	if (_hasFlyoutCreated)
	{
		ShowWindowAsync(HWndDUI, SW_MINIMIZE);
		ShowWindowAsync(HWndHost, SW_MINIMIZE);
		ShowWindowAsync(HWndHost, SW_FORCEMINIMIZE);
	}
}

void FlyoutHandler::ShowNativeFlyout()
{
	ShowWindowAsync(HWndDUI, SW_RESTORE);
}

void FlyoutHandler::VerifyNativeFlyoutCreated()
{
	if (!_hasFlyoutCreated)
	{
		GetAllInfos();
		Rehook();
	}
}