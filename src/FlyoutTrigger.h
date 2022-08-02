#pragma once

struct Measure;
class FlyoutHandler;
class FlightModeWatcher;

enum class TriggerType {
	None,
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
	FlyoutTrigger(Measure*);
	~FlyoutTrigger();

	static FlyoutTrigger* instance;
	static FlyoutHandler* parentHandler;
	static TriggerType triggerType;
	static UINT shellMessageHookId;
	bool isHookActive = false;

	void RegisterShellHook(Measure*);
	void Hook(Measure*);
	void Unhook();
private:
	HHOOK skinHook = NULL;
	
	static void TriggerFlyout(TriggerType);
	static LRESULT CALLBACK SkinWndProc(int, WPARAM, LPARAM);
};

