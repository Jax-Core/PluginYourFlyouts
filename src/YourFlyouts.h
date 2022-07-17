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

	std::wstring volumeChangeAction;
	std::wstring brightnessChangeAction;
	std::wstring mediaChangeAction;

	Measure() :
		rm(NULL),
		skin(NULL),
		id(NULL),
		volumeChangeAction(L""),
		brightnessChangeAction(L""),
		mediaChangeAction(L"")
	{}
};