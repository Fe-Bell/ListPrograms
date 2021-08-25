#include "InstalledPrograms.h"
#include <algorithm>
#include <regex>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Util.h"

InstalledPrograms::InstalledPrograms(void)
{
}

InstalledPrograms::~InstalledPrograms(void)
{
}

void InstalledPrograms::GetInstalledPrograms(std::vector<Software>& v, const bool& IncludeUpdates)
{
	GetInstalledProgramsImp(v, IncludeUpdates);
}

void InstalledPrograms::GetInstalledProgramsImp(std::vector<Software>& v, const bool& IncludeUpdates)
{
	RegistryKey* classesKey = RegistryKey::HKLM().OpenSubKey(L"Software\\Classes\\Installer\\Products");

	// The Classes\\Installer\\Products key is shared 
	// Documentation Here http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

	RegistryKey* wow64UninstallKey = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	GetUninstallKeyPrograms(v, wow64UninstallKey, classesKey, IncludeUpdates);

	RegistryKey* uninstallKey = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	GetUninstallKeyPrograms(v, uninstallKey, classesKey, IncludeUpdates);

	std::vector<std::wstring> subkeys = RegistryKey::HKU().GetSubKeyNames();
	
	for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
	{
		// These Two Keys are Both Shared
		// Documentation Here http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

		std::wstring uninstallsubs = (*it) + L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
		RegistryKey* UninstallKey = RegistryKey::HKU().OpenSubKey(uninstallsubs);
		GetUninstallKeyPrograms(v, UninstallKey, classesKey, IncludeUpdates);
		if(UninstallKey)
			delete UninstallKey;

		std::wstring installersubs = (*it) + L"\\Software\\Microsoft\\Installer\\Products";
		RegistryKey* InstallerKey = RegistryKey::HKU().OpenSubKey(installersubs);
		GetUserInstallerKeyPrograms(v, InstallerKey);
		if(InstallerKey)
			delete InstallerKey;
	}

	if(uninstallKey)
		delete uninstallKey;
	if(wow64UninstallKey)
		delete wow64UninstallKey;
	if(classesKey)
		delete classesKey;
	std::sort(v.begin(), v.end(), [](Software& s1, Software& s2)->bool
		{
			std::transform(s1.DisplayName.begin(), s1.DisplayName.end(), s1.DisplayName.begin(), ::tolower);
			std::transform(s2.DisplayName.begin(), s2.DisplayName.end(), s2.DisplayName.begin(), ::tolower);
			return s1.DisplayName.compare(s2.DisplayName) < 0;
		});	
}

void InstalledPrograms::GetUserInstallerKeyPrograms(std::vector<Software>& v, RegistryKey* uInstallerKey)
{
	if(uInstallerKey==NULL)
		return;

	RegistryKey * userData32 = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");
	RegistryKey * userData64 = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");

	if(userData32==NULL && userData64==NULL)
		return;

	RegistryKey* userDatas[2] = {userData32, userData64};
	for(int two = 0; two < 2; two++)
	{
		RegistryKey * userData = userDatas[two];
		if(userData==NULL)
			continue;

		std::vector<std::wstring> subkeys = uInstallerKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
		{
			std::vector<std::wstring> userDataSubKeys = userData->GetSubKeyNames();
			bool isProductFound = false;
			for(std::vector<std::wstring>::iterator userdatait = userDataSubKeys.begin(); userdatait!=userDataSubKeys.end(); userdatait++) 
			{
				//Ignore the LocalSystem account
				if (userdatait->compare(L"S-1-5-18")==0) {
					continue;
				}
			
				RegistryKey * productsKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products"), Arch_e::UnKnown);
                
				if ((productsKey != NULL)) 
				{
					std::vector<std::wstring> lmProductGuids = productsKey->GetSubKeyNames();
					for(std::vector<std::wstring>::iterator productit = lmProductGuids.begin(); productit!=lmProductGuids.end(); productit++) 
					{
						if (productit->compare(*it)==0) 
						{	
							RegistryKey * userDataProgramKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products\\").append(*productit).append(L"\\InstallProperties"), Arch_e::UnKnown);
							if(userDataProgramKey!=NULL)
							{
								if ( userDataProgramKey->GetValue(L"SystemComponent").compare(L"")==0 || _wtoi(userDataProgramKey->GetValue(L"SystemComponent").c_str())!=1 ) 
								{
									RegistryKey * temp = uInstallerKey->OpenSubKey(*it, uInstallerKey->KeyArch);

									// Name 
									std::wstring name1 = temp->GetValue(L"ProductName");
									std::wstring name2 = userDataProgramKey->GetValue(L"DisplayName");
									std::wstring name = L"";
									if(name1.compare(L"")==0)
										name = name2;
									else
										name = name1;

									// Version
									std::wstring progVersion = userDataProgramKey->GetValue(L"DisplayVersion");

									// InstallLocation
									std::wstring installLocation = userDataProgramKey->GetValue(L"InstallLocation");

									// Icon
									std::wstring icon1 = temp->GetValue(L"ProductIcon");
									std::wstring icon2 = userDataProgramKey->GetValue(L"DisplayIcon");
									std::wstring icon = L"";
									if(icon1.compare(L"")==0)
										icon = icon2;
									else
										icon = icon1;

									if ( name.compare(L"")!=0 ) 
									{
										auto sw = Software(name, progVersion, installLocation, icon, userData->KeyArch);
										Util::AddToList(v, sw);
										isProductFound = true;
									}
									delete temp;
								}
								delete userDataProgramKey;
							}
							break;
						}
					}
					if(isProductFound) 
					{
						break; // TODO: might not be correct. Was : Exit For
					}
					delete productsKey;
				}
			}
		}
		delete userData;
	}
}

void InstalledPrograms::GetUninstallKeyPrograms(std::vector<Software>& v, RegistryKey* UninstallKey, RegistryKey* ClassesKey, const bool& IncludeUpdates)
{
    //Make sure the key exists
    if (UninstallKey != NULL)
	{
        //Loop through all subkeys (each one represents an installed program)
		std::vector<std::wstring> uninstallSubKeys = UninstallKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator subKeyName = uninstallSubKeys.begin(); subKeyName!=uninstallSubKeys.end(); subKeyName++) 
		{
			RegistryKey * currentSubKey = UninstallKey->OpenSubKey(*subKeyName, UninstallKey->KeyArch);
			if (currentSubKey == NULL)
				continue;

            int isSystemComponent = 1;
            bool errorCheckingSystemComponent = false;
            
			std::wstring issystemcomponent = currentSubKey->GetValue(L"SystemComponent");
			if( issystemcomponent.compare(L"")==0 || _wtoi(issystemcomponent.c_str())!=1 )
				isSystemComponent = 0;
            
            if (isSystemComponent != 1) 
			{
                //If the WindowsInstaller flag is set then add the key name to our list of Windows Installer GUIDs
                if (currentSubKey->GetValue(L"WindowsInstaller").compare(L"")==0 || _wtoi(currentSubKey->GetValue(L"WindowsInstaller").c_str())!= 1) 
				{
                    std::wregex windowsUpdateRegEx(L"KB[0-9]{6}$");
                    
					// Release Type
					std::wstring programReleaseType = currentSubKey->GetValue(L"ReleaseType");

					// Version
                    std::wstring progVersion = currentSubKey->GetValue(L"DisplayVersion");
					// Name 
					std::wstring name = currentSubKey->GetValue(L"DisplayName");
					// InstallLocation
					std::wstring installLocation = currentSubKey->GetValue(L"InstallLocation");
                    // Icon
					std::wstring icon = currentSubKey->GetValue(L"DisplayIcon");

					//Check to see if this program is classed as an update
					if (regex_match(*subKeyName, windowsUpdateRegEx) == true || \
						currentSubKey->GetValue(L"ParentKeyName").compare(L"") != 0 ||\
						programReleaseType.compare(L"Security Update")==0 ||\
						programReleaseType.compare(L"Update Rollup")==0 ||\
						programReleaseType.compare(L"Hotfix")==0 )
					{
                        if (IncludeUpdates) 
						{
                            //Add the program to our list if we are including updates in this search
                            if ( name.compare(L"")!=0 ) 
							{
								Software sw = Software(name, progVersion, installLocation, icon, UninstallKey->KeyArch);
								Util::AddToList(v, sw);
                            }
                        }
                    } 
					else 
					{
						std::wstring uninstallvalue = currentSubKey->GetValue(L"UninstallString");
						if (uninstallvalue.compare(L"")!=0) 
						{
                            if ( name.compare(L"")!=0 ) 
							{
								Software sw = Software(name, progVersion, installLocation, icon, UninstallKey->KeyArch);
								Util::AddToList(v, sw);
                            }
                        }
                    }
                } 

				//If WindowsInstaller
				else 
				{
                    // Name
					std::wstring name1 = L"";
					std::wstring icon1 = L"";
                    std::wstring msiKeyName = Util::GetInstallerKeyNameFromGuid(*subKeyName);
					RegistryKey * crGuidKey = ClassesKey->OpenSubKey(msiKeyName,ClassesKey->KeyArch);
                    if (crGuidKey != NULL)
					{
                        name1 = crGuidKey->GetValue(L"ProductName");
						icon1 = crGuidKey->GetValue(L"ProductIcon");
						delete crGuidKey;
                    }

					std::wstring name2 = currentSubKey->GetValue(L"DisplayName");
					std::wstring name = L"";
					if(name1.compare(L"")==0)
						name = name2;
					else
						name = name1;

                    // Version
					std::wstring progVersion = currentSubKey->GetValue(L"DisplayVersion");
                    
					// InstallLocation
					std::wstring installLocation = currentSubKey->GetValue(L"InstallLocation");

					// Icon
					std::wstring icon2 = currentSubKey->GetValue(L"DisplayIcon");
					std::wstring icon = L"";
					if(icon1.compare(L"")==0)
						icon = icon2;
					else
						icon = icon1;

					if (name.compare(L"")!=0)
					{
						Software sw = Software(name, progVersion, installLocation, icon, UninstallKey->KeyArch);
						Util::AddToList(v, sw);
                    }
                }
            }
        }
    }
}