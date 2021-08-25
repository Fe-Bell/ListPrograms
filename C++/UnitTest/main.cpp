#include <iostream>
#include "InstalledPrograms.h"
#include <vector>

int main()
{
	std::vector<Software>* list = InstalledPrograms::GetInstalledPrograms(false);
	for(std::vector<Software>::iterator iter = list->begin(); iter!=list->end(); iter++)
	{
		std::wcout << iter->DisplayName << L"  " << iter->InstallLocation << L"  " << iter->Version << L"  " << iter->Icon << L"  " << (int)iter->Architecture << std::endl;
	}
	delete list;
}