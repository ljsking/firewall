#pragma once

// FirewallInstaller.h : main header file for FirewallInstaller.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols


// CFirewallInstallerApp : See FirewallInstaller.cpp for implementation.

class CFirewallInstallerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

