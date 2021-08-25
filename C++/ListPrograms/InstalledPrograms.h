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

	static void GetInstalledPrograms(std::vector<Software>& v, const bool& IncludeUpdates);

private:
	static void GetInstalledProgramsImp(std::vector<Software>& v, const bool& IncludeUpdates);
	static void GetUninstallKeyPrograms(std::vector<Software>& v, RegistryKey* UninstallKey, RegistryKey* ClassesKey, const bool& IncludeUpdates);
	static void GetUserInstallerKeyPrograms(std::vector<Software>& v, RegistryKey* uInstallerKey);
};

#endif