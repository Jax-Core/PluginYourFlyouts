#include "YourFlyouts.h"
#include "FlyoutTrigger.h"
#include "FlyoutHandler.h"
#include "FlightModeWatcher.h"

extern HINSTANCE hInstance;

FlyoutTrigger* FlyoutTrigger::instance;
FlyoutHandler* FlyoutTrigger::parentHandler;
TriggerType FlyoutTrigger::triggerType = TriggerType::None;
UINT FlyoutTrigger::shellMessageHookId;

FlyoutTrigger::FlyoutTrigger(Measure* measure)
{
	instance = this;
	RegisterShellHook(measure);
	Hook(measure);
}

FlyoutTrigger::~FlyoutTrigger()
{
	Unhook();
}

void FlyoutTrigger::RegisterShellHook(Measure* measure)
{
	if (!RegisterShellHookWindow(RmGetSkinWindow(measure->rm)))
		return;
	shellMessageHookId = RegisterWindowMessage(L"SHELLHOOK");
	parentHandler->Log(LOG_DEBUG, L"Shell hook added!");
}

void FlyoutTrigger::Hook(Measure* measure)
{
	if (isHookActive)
		return;
	DWORD threadId = GetWindowThreadProcessId(RmGetSkinWindow(measure->rm), NULL);
	if ((skinHook = SetWindowsHookEx(WH_GETMESSAGE, SkinWndProc, hInstance, threadId)) != NULL)
	{
		parentHandler->Log(LOG_DEBUG, L"Set message hook!");
		isHookActive = true;
	}
	else {
		parentHandler->Log(LOG_ERROR, L"Couldn't message hook!");
	}

}

void FlyoutTrigger::Unhook()
{
	if (isHookActive) {
		UnhookWindowsHookEx(skinHook);
		parentHandler->Log(LOG_DEBUG, L"Get message hook removed!");
		isHookActive = false;
	}
}

void FlyoutTrigger::TriggerFlyout(TriggerType tType)
{
	triggerType = tType;
}

LRESULT FlyoutTrigger::SkinWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	MSG* msg = (MSG*)lParam;
	if (msg->message != shellMessageHookId)
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	// RmLogF(instance->parentHandler->measures[0]->rm, LOG_NOTICE, L"Message recieved: %ld", msg->wParam);

	switch (msg->wParam) {
	case 55:
		TriggerFlyout(TriggerType::Brightness);
		break;
	case 12:
	{
		switch ((LONG)msg->lParam) {
		case (LONG)TriggerReason::MediaNext:
		case (LONG)TriggerReason::MediaPrev:
		case (LONG)TriggerReason::MediaPlayPause:
		case (LONG)TriggerReason::MediaStop:
			TriggerFlyout(TriggerType::Media);
			break;
		case (LONG)TriggerReason::VolumeDown:
		case (LONG)TriggerReason::VolumeUp:
		case (LONG)TriggerReason::VolumeMute:
			TriggerFlyout(TriggerType::Volume);
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}