#include "Util.h"
#include <locale>
#include <codecvt>

void Util::AddToList(std::vector<Software>& v, Software& software)
{
	int index = -1;
	for (size_t i = 0; i < v.size(); i++)
	{
		if (v.at(i).DisplayName.compare(software.DisplayName) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		v.push_back(software);
	else
	{
		Software duplicate = v.at(index);

		// Merge Architecture
		if (software.Architecture != Arch_e::UnKnown && duplicate.Architecture != Arch_e::UnKnown && duplicate.Architecture != software.Architecture)
		{
			v.push_back(software);
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
			v.push_back(software);
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
			v.push_back(software);
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
			v.push_back(software);
			return;
		}
		else
		{
			if (software.Version.compare(L"") == 0)
				software.Version = duplicate.Version;
		}
		v.erase(v.begin() + index);
		v.push_back(software);
	}
}

std::wstring Util::GetInstallerKeyNameFromGuid(std::wstring guid)
{
	ReplaceAll(guid, L"{", L"");
	ReplaceAll(guid, L"}", L"");
	std::vector<std::wstring> msiNameParts = Split(guid, L'-');
	std::wstring msiName;
	//Just reverse the first 3 parts
	for (int i = 0; i <= 2; i++) 
	{
		msiName.append(ReverseString(msiNameParts[i]));
	}
	//For the last 2 parts, reverse each character pair
	for (int j = 3; j <= 4; j++)
	{
		for (size_t i = 0; i <= msiNameParts[j].length() - 1; i++)
		{
			msiName.append(std::wstring(1, msiNameParts[j].c_str()[i + 1]));
			msiName.append(std::wstring(1, msiNameParts[j].c_str()[i]));
			i += 1;
		}
	}
	return msiName;
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

std::vector<std::wstring> Util::Split(const std::wstring& text, const wchar_t& delimiter)
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

const std::string Util::ToString(const Arch_e& a)
{
	switch (a)
	{
	default:
	case Arch_e::UnKnown:
		return "Unknown";
	case Arch_e::X86:
		return "X86";
	case Arch_e::X64:
		return "X64";
	}
}

const std::wstring Util::ToWString(const Arch_e& a)
{
	std::string s = ToString(a);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//std::string narrow = converter.to_bytes(wide_utf16_source_string);
	std::wstring wide = converter.from_bytes(s);
	return wide;
}