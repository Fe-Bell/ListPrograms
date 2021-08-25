#include <iostream>
#include "InstalledPrograms.h"
#include <vector>
#include "Util.h"

int main()
{
	std::vector<Software> list;
	InstalledPrograms::GetInstalledPrograms(list, false);
	for(auto& iter : list)
	{
		std::wcout << iter.DisplayName << L"  " << iter.InstallLocation << L"  " << iter.Version << L"  " << iter.Icon << L"  " << Util::ToWString(iter.Architecture) << std::endl;
	}

	int i = getchar();
}