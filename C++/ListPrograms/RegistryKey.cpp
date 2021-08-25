#include "RegistryKey.h"

RegistryKey::RegistryKey(HKEY hkey, Arch arch)
{
	this->hkey = hkey;
	this->KeyArch = arch;
}

RegistryKey::~RegistryKey(void)
{
	if( this->hkey == HKEY_LOCAL_MACHINE || this->hkey == HKEY_USERS )
		return;
	RegCloseKey(this->hkey);
}

RegistryKey* RegistryKey::OpenSubKey64(std::wstring subkey)
{
	HKEY hKey;
	if (RegOpenKeyEx(this->hkey, subkey.c_str(), 0, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}
	else
	{
		return new RegistryKey(hKey, Arch::X64);
	}
}

RegistryKey* RegistryKey::OpenSubKey32(std::wstring subkey)
{
	HKEY hKey;
	if (RegOpenKeyEx(this->hkey, subkey.c_str(), 0, KEY_READ|KEY_WOW64_32KEY, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}
	else
	{
		return new RegistryKey(hKey, Arch::X86);
	}
}

RegistryKey* RegistryKey::OpenSubKey(std::wstring subkey)
{
	HKEY hKey;
	if (RegOpenKeyEx(this->hkey, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}
	else
	{
		return new RegistryKey(hKey, Arch::UnKnown);
	}
}

RegistryKey* RegistryKey::OpenSubKey(std::wstring subkey, Arch a)
{
	HKEY hKey;
	DWORD FLAG;
	if(a== Arch::X64)
		FLAG = KEY_WOW64_64KEY;
	else if(a== Arch::X86)
		FLAG = KEY_WOW64_32KEY;
	else
		FLAG = 0;

	if (RegOpenKeyEx(this->hkey, subkey.c_str(), 0, KEY_READ|FLAG, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}
	else
	{
		return new RegistryKey(hKey, a);
	}
}

RegistryKey& RegistryKey::HKLM()
{
	static RegistryKey Key(HKEY_LOCAL_MACHINE, Arch::UnKnown);
	return Key;
}

RegistryKey& RegistryKey::HKU()
{
	static RegistryKey Key(HKEY_USERS, Arch::UnKnown);
	return Key;
}

std::vector<std::wstring> RegistryKey::GetSubKeyNames()
{
	std::vector<std::wstring> ret;
	LONG lRet;
	DWORD dwIndex = 0;
	DWORD cbName = MAX_PATH;
	WCHAR szSubKeyName[MAX_PATH];
	while( (lRet = RegEnumKeyEx(this->hkey, dwIndex, szSubKeyName, &cbName, NULL,NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS )
	{
		if(lRet == ERROR_SUCCESS)
		{
			ret.push_back(std::wstring(szSubKeyName));
		}
		cbName = MAX_PATH;
		dwIndex++;
	}
	return ret;
}

std::wstring RegistryKey::GetValue(std::wstring query)
{
	WCHAR buffer[MAX_PATH];
	DWORD dwSize = sizeof(buffer);
	DWORD dwType;
	if (RegQueryValueEx(this->hkey, query.c_str(), NULL, &dwType, (LPBYTE)&buffer, &dwSize) == ERROR_SUCCESS)
    {
		if(dwType==REG_DWORD)
		{
			DWORD * ret = (DWORD*)buffer;
			//std::wstring s = std::wstring(_itow((*ret), Value, 10));
			errno_t rc = _itow_s(*ret, buffer, 255, 10);
			if (rc != 0)
			{
				char b[255];
				sprintf_s(b, 255, "GetValue() -> _itow_s returned %d", rc);
			}

			return std::wstring(buffer);
		}
		else if (dwType == REG_SZ)
		{
			return std::wstring(buffer);
		}
		else if(dwType == REG_EXPAND_SZ)
		{
			WCHAR Expanded[MAX_PATH];
			ExpandEnvironmentStrings(buffer, Expanded, MAX_PATH);
			return std::wstring(Expanded);
		}
		else
		{
			return std::wstring(buffer);
		}
    }
	else
	{
		return L"";
	}
}