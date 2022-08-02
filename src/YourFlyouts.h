#pragma once
#include <Windows.h>
#include "../rmapi/RainmeterAPI.h"
#include "string"
#include "vector"

struct Measure
{
	void* rm;
	void* skin;
	int id;

	std::wstring brightnessChangeAction;
	std::wstring flightModeChangeAction;
	std::wstring mediaChangeAction;
	std::wstring volumeChangeAction;

	Measure() :
		rm(NULL),
		skin(NULL),
		id(NULL),
		brightnessChangeAction(L""),
		flightModeChangeAction(L""),
		mediaChangeAction(L""),
		volumeChangeAction(L"")
	{}
};