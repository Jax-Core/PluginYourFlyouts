#pragma once
#include "YourFlyouts.h"

class FlyoutHandler;

enum class TriggerType {
	Volume,
	Media,
	Brightness
};

enum class TriggerReason {
	VolumeUp = 655360,
	VolumeDown = 589824,
	VolumeMute = 524288,
	MediaPlayPause = 917504,
	MediaNext = 720896,
	MediaPrev = 786432,
	MediaStop = 851968
};

class FlyoutTrigger
{
public:
	FlyoutTrigger();
	~FlyoutTrigger();

	static FlyoutTrigger* instance;
	static FlyoutHandler* parentHandler;
	static UINT shellMessageHookId;
	bool isHookActive = false;

	void RegisterShellHook(Measure*);
	void Hook(Measure*);
	void Unhook();
private:
	HHOOK skinHook = NULL;
	
	static void TriggerFlyout(TriggerType);
	static LRESULT CALLBACK GetMsgProc(int, WPARAM, LPARAM);
	static LRESULT CALLBACK SkinWndProc(int, WPARAM, LPARAM);
	void OnVolumeChanged();
	void OnBrightnessChanged();
	void OnMediaChanged();
};

