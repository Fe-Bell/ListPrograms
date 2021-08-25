#ifndef INSTALLED_PROGRAMS
#define INSTALLED_PROGRAMS

#include <vector>
#include "RegistryKey.h"

class Software
{
public:
	std::wstring DisplayName;
	std::wstring InstallLocation;
	std::wstring Version;
	Arch Architecture; // 32 or 64
	std::wstring Icon;
	Software(std::wstring, std::wstring, std::wstring, std::wstring, Arch);
};


class InstalledPrograms
{
public:
	InstalledPrograms(void);
	~InstalledPrograms(void);
	static std::vector<Software>* GetInstalledPrograms(bool IncludeUpdates);
private:
	static std::vector<Software>* GetInstalledProgramsImp(bool IncludeUpdates);
	static std::vector<Software>* GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>*, bool IncludeUpdates);
	static std::vector<Software>* GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList);
};

#endif