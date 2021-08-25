#include <iostream>
#include "InstalledPrograms.h"
#include <vector>

int main()
{
	std::vector<Software> list;
	InstalledPrograms::GetInstalledPrograms(list, false);
	for(auto& iter : list)
	{
		std::wcout << iter.DisplayName << L"  " << iter.InstallLocation << L"  " << iter.Version << L"  " << iter.Icon << L"  " << (int)iter.Architecture << std::endl;
	}

	int i = getchar();
}