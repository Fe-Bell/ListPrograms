#pragma once
#include <string>
#include "Architecture.h"

class Software
{
public:
	std::wstring DisplayName;
	std::wstring InstallLocation;
	std::wstring Version;
	Arch_e Architecture; // 32 or 64
	std::wstring Icon;

	Software(const std::wstring& displayName, const std::wstring& installLocation, const std::wstring& version, const std::wstring& icon, const Arch_e& architecture);
	~Software();
};
