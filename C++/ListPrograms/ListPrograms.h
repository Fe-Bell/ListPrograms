#pragma once
#include "Software.h"

extern "C"
{
	__declspec(dllexport) Software* GetInstalledPrograms(size_t& size, const bool& includeUpdates);
}