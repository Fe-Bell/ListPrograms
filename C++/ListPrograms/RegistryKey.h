#ifndef REGISTRY_KEY
#define REGISTRY_KEY

#include <Windows.h>
#include <string>
#include <vector>
#include "Architecture.h"

class RegistryKey
{
public:
	Arch_e KeyArch; // Indicate whether the Key is a 64 bit or 32 bit Key.

	~RegistryKey();

	RegistryKey* OpenSubKey64(const std::wstring& subkey);
	RegistryKey* OpenSubKey32(const std::wstring& subkey);
	RegistryKey* OpenSubKey(const std::wstring& subkey);
	RegistryKey* OpenSubKey(const std::wstring& subkey, const Arch_e& a);

	std::vector<std::wstring> GetSubKeyNames();
	std::wstring GetValue(const std::wstring& query);

	static RegistryKey& HKLM();
	static RegistryKey& HKU();

private:
	HKEY hkey;
	RegistryKey(HKEY, const Arch_e&); // The constructor is private, not explicit initilization is available.
};

#endif