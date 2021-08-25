#pragma once
#include <string>
#include <vector>
#include "Software.h"

class Util
{
public:
	static std::wstring ReverseString(std::wstring& input);
	static void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);
	static std::vector<std::wstring> Split(const std::wstring& text, const wchar_t& delimiter);
	static std::wstring GetInstallerKeyNameFromGuid(std::wstring GuidName);
	static void AddToList(std::vector<Software>& v, Software& software);
	static const std::string ToString(const Arch_e& a);	
	static const std::wstring ToWString(const Arch_e& a);
};

