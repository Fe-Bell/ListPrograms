#include "Util.h"

void Util::AddToList(std::vector<Software>* TheList, Software software)
{
	int index = -1;
	for (size_t i = 0; i < TheList->size(); i++)
	{
		if (TheList->at(i).DisplayName.compare(software.DisplayName) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		TheList->push_back(software);
	else
	{
		Software duplicate = TheList->at(index);

		// Merge Architecture
		if (software.Architecture != Arch_e::UnKnown && duplicate.Architecture != Arch_e::UnKnown && duplicate.Architecture != software.Architecture)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if (software.Architecture == Arch_e::UnKnown)
				software.Architecture = duplicate.Architecture;
		}

		// Merge Icon
		if (software.Icon.compare(L"") != 0 && duplicate.Icon.compare(L"") != 0 && software.Icon.compare(duplicate.Icon) != 0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if (software.Icon.compare(L"") == 0)
				software.Icon = duplicate.Icon;
		}

		// Merge Location
		if (software.InstallLocation.compare(L"") != 0 && duplicate.InstallLocation.compare(L"") != 0 && software.InstallLocation.compare(duplicate.InstallLocation) != 0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if (software.InstallLocation.compare(L"") == 0)
				software.InstallLocation = duplicate.InstallLocation;
		}

		// Merge Version
		if (software.Version.compare(L"") != 0 && duplicate.Version.compare(L"") != 0 && software.Version.compare(duplicate.Version) != 0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if (software.Version.compare(L"") == 0)
				software.Version = duplicate.Version;
		}
		TheList->erase(TheList->begin() + index);
		TheList->push_back(software);
	}
}

std::wstring Util::GetInstallerKeyNameFromGuid(std::wstring GuidName)
{
	ReplaceAll(GuidName, L"{", L"");
	ReplaceAll(GuidName, L"}", L"");
	std::vector<std::wstring> MsiNameParts = Split(GuidName, L'-');
	std::wstring MsiName;
	//Just reverse the first 3 parts
	for (int i = 0; i <= 2; i++) {
		MsiName.append(ReverseString(MsiNameParts[i]));
	}
	//For the last 2 parts, reverse each character pair
	for (int j = 3; j <= 4; j++)
	{
		for (size_t i = 0; i <= MsiNameParts[j].length() - 1; i++)
		{
			MsiName.append(std::wstring(1, MsiNameParts[j].c_str()[i + 1]));
			MsiName.append(std::wstring(1, MsiNameParts[j].c_str()[i]));
			i += 1;
		}
	}
	return MsiName;
}

std::wstring Util::ReverseString(std::wstring& input)
{
	std::reverse(input.begin(), input.end());
	return input;
}

void Util::ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

std::vector<std::wstring> Util::Split(const std::wstring& text, wchar_t delimiter)
{
	std::vector<std::wstring> result;

	std::wstring::size_type start = 0;
	std::wstring::size_type end = text.find(delimiter, start);

	while (end != std::wstring::npos)
	{
		std::wstring token = text.substr(start, end - start);

		result.push_back(token);

		start = end + 1;
		end = text.find(delimiter, start);
	}

	result.push_back(text.substr(start));

	return result;
}