#include "YourFlyouts.h"
#include "FlyoutHandler.h"

HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD ul_reason_for_call, LPWORD lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		hInstance = hinst;

		// Disable DLL_THREAD_ATTACH and DLL_THREAD_DETACH notification calls : saves resources
		DisableThreadLibraryCalls((HMODULE)hinst);
		break;
	}

	return TRUE;
}

FlyoutHandler* flyoutHandler = NULL;

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;

	if (!flyoutHandler) {
		flyoutHandler = new FlyoutHandler;
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;
	measure->rm = rm;
	measure->skin = RmGetSkin(rm);

	flyoutHandler->AddMeasure(measure);

	measure->brightnessChangeAction = RmReadString(rm, L"BrightnessChangeAction", L"");
	measure->flightModeChangeAction = RmReadString(rm, L"FlightModeChangeAction", L"");
	measure->mediaChangeAction = RmReadString(rm, L"MediaChangeAction", L"");
	measure->volumeChangeAction = RmReadString(rm, L"VolumeChangeAction", L"");
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;
	flyoutHandler->Update(measure->id);
	return 0.0;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	flyoutHandler->RemoveMeasure(measure);
	delete measure;
	if (flyoutHandler->measures.empty()) {
		delete flyoutHandler;
		flyoutHandler = NULL;
	}
}
