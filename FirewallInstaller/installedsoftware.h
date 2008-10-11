#pragma once

#include <afxcoll.h>

#define	IS_KEY		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define	IS_KEY_LEN	256
#define IS_DISPLAY	_T("DisplayName")

class CInstalledSoftware
{
public:
	CStringArray m_aPrograms;
public:
	CInstalledSoftware(void)
	{
		BuildList();
	}

	void BuildList(void)
	{
		// Build a list of installed applications by enumerating
		//	HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall
		//	and fetching "DisplayName" entry

		HKEY hKey;
		if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, IS_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			return;

		DWORD dwIndex = 0;
		LONG lRet;
		DWORD cbName = IS_KEY_LEN;
		TCHAR szSubKeyName[IS_KEY_LEN];
		
		while ((lRet = ::RegEnumKeyEx(hKey, dwIndex, szSubKeyName, &cbName, NULL,
			NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
		{
			// Do we have a key to open?
			if (lRet == ERROR_SUCCESS)
			{
				// Open the key and get the value
				HKEY hItem;
				if (::RegOpenKeyEx(hKey, szSubKeyName, 0, KEY_READ, &hItem) != ERROR_SUCCESS)
					continue;
				// Opened - look for "DisplayName"
				TCHAR szDisplayName[IS_KEY_LEN];
				DWORD dwSize = sizeof(szDisplayName);
				DWORD dwType;

				if (::RegQueryValueEx(hItem, IS_DISPLAY, NULL, &dwType,
					(LPBYTE)&szDisplayName, &dwSize) == ERROR_SUCCESS)
				{
					// Add to the main array
					m_aPrograms.Add(szDisplayName);					
				}
				::RegCloseKey(hItem);
			}
			dwIndex++;
			cbName = IS_KEY_LEN;
		}
		::RegCloseKey(hKey);
	}
};
