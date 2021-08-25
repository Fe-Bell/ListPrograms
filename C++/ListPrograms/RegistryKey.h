#ifndef REGISTRY_KEY
#define REGISTRY_KEY

#include <Windows.h>
#include <string>
#include <vector>

enum class Arch
{
	UnKnown = 0,
	X86 = 1,
	X64 = 2
};

class RegistryKey
{
public:
	~RegistryKey(void);
	RegistryKey* OpenSubKey64(std::wstring subkey);
	RegistryKey* OpenSubKey32(std::wstring subkey);
	RegistryKey* OpenSubKey(std::wstring subkey);
	RegistryKey* OpenSubKey(std::wstring subkey, Arch a);

	std::vector<std::wstring> GetSubKeyNames();
	std::wstring GetValue(std::wstring query);

	static RegistryKey& HKLM();
	static RegistryKey& HKU();

	Arch KeyArch; // Indicate whether the Key is a 64 bit or 32 bit Key.

private:
	HKEY hkey;
	RegistryKey(HKEY, Arch); // The constructor is private, not explicit initilization is available.
};

#endif