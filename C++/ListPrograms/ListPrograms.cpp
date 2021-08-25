#include "ListPrograms.h"
#include "InstalledPrograms.h"

Software* GetInstalledPrograms(size_t& size, const bool& includeUpdates)
{
	std::vector<Software> v;
	InstalledPrograms::GetInstalledPrograms(v, includeUpdates);
	
	size = v.size();
	if (size <= 0)
	{
		return nullptr;
	}

	Software* arr = new Software[size];
	for (size_t i = 0; i < size; i++)
	{
		arr[i] = v[i];
	}
	return arr;
}
