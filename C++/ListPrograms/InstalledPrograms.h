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

	static void GetInstalledPrograms(std::vector<Software>& v, const bool& includeUpdates);

private:
	static void GetInstalledProgramsImp(std::vector<Software>& v, const bool& includeUpdates);
	static void GetUninstallKeyPrograms(std::vector<Software>& v, RegistryKey* uninstallKey, RegistryKey* classesKey, const bool& includeUpdates);
	static void GetUserInstallerKeyPrograms(std::vector<Software>& v, RegistryKey* userInstallerKey);
};

#endif