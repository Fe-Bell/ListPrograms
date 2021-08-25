#ifndef INSTALLED_PROGRAMS
#define INSTALLED_PROGRAMS

#include <vector>
#include "RegistryKey.h"
#include "Software.h"

class InstalledPrograms
{
public:
	InstalledPrograms();
	~InstalledPrograms();

	static std::vector<Software>* GetInstalledPrograms(const bool& IncludeUpdates);

private:
	static std::vector<Software>* GetInstalledProgramsImp(const bool& IncludeUpdates);
	static std::vector<Software>* GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>*, const bool& IncludeUpdates);
	static std::vector<Software>* GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList);
};

#endif