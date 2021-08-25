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

std::vector<Software>* InstalledPrograms::GetInstalledPrograms(const bool& IncludeUpdates)
{
	return GetInstalledProgramsImp(IncludeUpdates);
}

std::vector<Software>* InstalledPrograms::GetInstalledProgramsImp(const bool& IncludeUpdates)
{
    std::vector<Software>* SoftwareList = new std::vector<Software>();
	
	RegistryKey* ClassesKey = RegistryKey::HKLM().OpenSubKey(L"Software\\Classes\\Installer\\Products");

	// The Classes\\Installer\\Products key is shared 
	// Documentation Here http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

	RegistryKey* Wow64UninstallKey = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	SoftwareList = GetUninstallKeyPrograms(Wow64UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);

	RegistryKey* UninstallKey = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	SoftwareList = GetUninstallKeyPrograms(UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);

	std::vector<std::wstring> subkeys = RegistryKey::HKU().GetSubKeyNames();
	
	for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
	{
		// These Two Keys are Both Shared
		// Documentation Here http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

		std::wstring uninstallsubs = (*it) + L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
		RegistryKey* UninstallKey = RegistryKey::HKU().OpenSubKey(uninstallsubs);
		SoftwareList = GetUninstallKeyPrograms(UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);
		if(UninstallKey)
			delete UninstallKey;

		std::wstring installersubs = (*it) + L"\\Software\\Microsoft\\Installer\\Products";
		RegistryKey* InstallerKey = RegistryKey::HKU().OpenSubKey(installersubs);
		SoftwareList = GetUserInstallerKeyPrograms(InstallerKey, SoftwareList);
		if(InstallerKey)
			delete InstallerKey;
	}

	if(UninstallKey)
		delete UninstallKey;
	if(Wow64UninstallKey)
		delete Wow64UninstallKey;
	if(ClassesKey)
		delete ClassesKey;
	std::sort(SoftwareList->begin(), SoftwareList->end(), [](Software s1, Software s2)->bool{std::transform(s1.DisplayName.begin(), s1.DisplayName.end(), s1.DisplayName.begin(), ::tolower);std::transform(s2.DisplayName.begin(), s2.DisplayName.end(), s2.DisplayName.begin(), ::tolower);return s1.DisplayName.compare(s2.DisplayName)<0;} );
	return SoftwareList;

}

std::vector<Software>* InstalledPrograms::GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList)
{
	if(uInstallerKey==NULL)
		return ExistingProgramList;

	RegistryKey * UserData32 = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");
	RegistryKey * UserData64 = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");

	if(UserData32==NULL && UserData64==NULL)
		return ExistingProgramList;


	RegistryKey* UserDatas[2] = {UserData32, UserData64};
	for(int two = 0; two < 2; two++)
	{
		RegistryKey * UserData = UserDatas[two];
		if(UserData==NULL)
			continue;

		std::vector<std::wstring> subkeys = uInstallerKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
		{
			std::vector<std::wstring> userDataSubKeys = UserData->GetSubKeyNames();
			bool ProductFound = false;
			for(std::vector<std::wstring>::iterator userdatait = userDataSubKeys.begin(); userdatait!=userDataSubKeys.end(); userdatait++) 
			{
				//Ignore the LocalSystem account
				if (userdatait->compare(L"S-1-5-18")==0) {
					continue;
				}
			
				RegistryKey * ProductsKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products"), Arch_e::UnKnown);
                
				if ((ProductsKey != NULL)) 
				{
					std::vector<std::wstring> LmProductGuids = ProductsKey->GetSubKeyNames();
					for(std::vector<std::wstring>::iterator productit = LmProductGuids.begin(); productit!=LmProductGuids.end(); productit++) 
					{
						if (productit->compare(*it)==0) 
						{	
							RegistryKey * UserDataProgramKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products\\").append(*productit).append(L"\\InstallProperties"), Arch_e::UnKnown);
							if(UserDataProgramKey!=NULL)
							{
								if ( UserDataProgramKey->GetValue(L"SystemComponent").compare(L"")==0 || _wtoi(UserDataProgramKey->GetValue(L"SystemComponent").c_str())!=1 ) 
								{
									RegistryKey * temp = uInstallerKey->OpenSubKey(*it, uInstallerKey->KeyArch);

									// Name 
									std::wstring Name1 = temp->GetValue(L"ProductName");
									std::wstring Name2 = UserDataProgramKey->GetValue(L"DisplayName");
									std::wstring Name = L"";
									if(Name1.compare(L"")==0)
										Name = Name2;
									else
										Name = Name1;

									// Version
									std::wstring ProgVersion = UserDataProgramKey->GetValue(L"DisplayVersion");

									// InstallLocation
									std::wstring InstallLocation = UserDataProgramKey->GetValue(L"InstallLocation");

									// Icon
									std::wstring Icon1 = temp->GetValue(L"ProductIcon");
									std::wstring Icon2 = UserDataProgramKey->GetValue(L"DisplayIcon");
									std::wstring Icon = L"";
									if(Icon1.compare(L"")==0)
										Icon = Icon2;
									else
										Icon = Icon1;

									if ( Name.compare(L"")!=0 ) 
									{
										Util::AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,Icon,UserData->KeyArch));
										ProductFound = true;
									}
									delete temp;
								}
								delete UserDataProgramKey;
							}
							break;
						}
					}
					if(ProductFound) 
					{
						break; // TODO: might not be correct. Was : Exit For
					}
					delete ProductsKey;
				}
			}
		}
		delete UserData;
	}
    return ExistingProgramList;
}

std::vector<Software>* InstalledPrograms::GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>* ExistingProgramList, const bool& IncludeUpdates)
{
    //Make sure the key exists
    if (UninstallKey != NULL)
	{
        //Loop through all subkeys (each one represents an installed program)
		std::vector<std::wstring> UninstallSubKeys = UninstallKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator SubKeyName = UninstallSubKeys.begin(); SubKeyName!=UninstallSubKeys.end(); SubKeyName++) 
		{
			RegistryKey * CurrentSubKey = UninstallKey->OpenSubKey(*SubKeyName, UninstallKey->KeyArch);
			if (CurrentSubKey == NULL)
				continue;

            int IsSystemComponent = 1;
            bool ErrorCheckingSystemComponent = false;
            
			std::wstring issystemcomponent = CurrentSubKey->GetValue(L"SystemComponent");
			if( issystemcomponent.compare(L"")==0 || _wtoi(issystemcomponent.c_str())!=1 )
				IsSystemComponent = 0;
            
            if (IsSystemComponent != 1) 
			{
                //If the WindowsInstaller flag is set then add the key name to our list of Windows Installer GUIDs
                if (CurrentSubKey->GetValue(L"WindowsInstaller").compare(L"")==0 || _wtoi(CurrentSubKey->GetValue(L"WindowsInstaller").c_str())!= 1) 
				{
                    std::wregex WindowsUpdateRegEx(L"KB[0-9]{6}$");
                    
					// Release Type
					std::wstring ProgramReleaseType = CurrentSubKey->GetValue(L"ReleaseType");

					// Version
                    std::wstring ProgVersion = CurrentSubKey->GetValue(L"DisplayVersion");
					// Name 
					std::wstring Name = CurrentSubKey->GetValue(L"DisplayName");
					// InstallLocation
					std::wstring InstallLocation = CurrentSubKey->GetValue(L"InstallLocation");
                    // Icon
					std::wstring Icon = CurrentSubKey->GetValue(L"DisplayIcon");

					//Check to see if this program is classed as an update
					if (regex_match(*SubKeyName, WindowsUpdateRegEx) == true || \
						CurrentSubKey->GetValue(L"ParentKeyName").compare(L"") != 0 ||\
						ProgramReleaseType.compare(L"Security Update")==0 ||\
						ProgramReleaseType.compare(L"Update Rollup")==0 ||\
						ProgramReleaseType.compare(L"Hotfix")==0 )
					{
                        if (IncludeUpdates) 
						{
                            //Add the program to our list if we are including updates in this search
                            if ( Name.compare(L"")!=0 ) 
							{
								Util::AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,Icon,UninstallKey->KeyArch));
                            }
                        }
                    } 
					else 
					{
						std::wstring uninstallvalue = CurrentSubKey->GetValue(L"UninstallString");
						if (uninstallvalue.compare(L"")!=0) 
						{
                            if ( Name.compare(L"")!=0 ) 
							{
								Util::AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,Icon,UninstallKey->KeyArch));
                            }
                        }
                    }
                } 

				//If WindowsInstaller
				else 
				{
                    // Name
					std::wstring Name1 = L"";
					std::wstring  Icon1 = L"";
                    std::wstring MsiKeyName = Util::GetInstallerKeyNameFromGuid(*SubKeyName);
					RegistryKey * CrGuidKey = ClassesKey->OpenSubKey(MsiKeyName,ClassesKey->KeyArch);
                    if (CrGuidKey != NULL)
					{
                        Name1 = CrGuidKey->GetValue(L"ProductName");
						Icon1 = CrGuidKey->GetValue(L"ProductIcon");
						delete CrGuidKey;
                    }
					std::wstring Name2 = CurrentSubKey->GetValue(L"DisplayName");
					std::wstring Name = L"";
					if(Name1.compare(L"")==0)
						Name = Name2;
					else
						Name = Name1;

                    // Version
					std::wstring ProgVersion = CurrentSubKey->GetValue(L"DisplayVersion");
                    

					// InstallLocation
					std::wstring InstallLocation = CurrentSubKey->GetValue(L"InstallLocation");

					// Icon
					std::wstring Icon2 = CurrentSubKey->GetValue(L"DisplayIcon");
					std::wstring Icon = L"";
					if(Icon1.compare(L"")==0)
						Icon = Icon2;
					else
						Icon = Icon1;

					if (Name.compare(L"")!=0)
					{
						Util::AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,Icon,UninstallKey->KeyArch));
                    }
                }
            }
        }
    }
    return ExistingProgramList;
}