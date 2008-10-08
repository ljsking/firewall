// FirewallInstaller.cpp : Implementation of CFirewallInstallerApp and DLL registration.

#include "stdafx.h"
#include "FirewallInstaller.h"
#include "cathelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CFirewallInstallerApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x2ADA0098, 0x3A05, 0x443F, { 0xB3, 0x5A, 0xFA, 0xDF, 0xEB, 0xFB, 0x72, 0x81 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

const CATID CATID_SafeForScripting     =
                {0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
const CATID CATID_SafeForInitializing  =
                {0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};

const GUID CDECL BASED_CODE _ctlid =
        {0x71cbe562, 0xfcae, 0x4cea, 0xa1, 0x53, 0x83, 0x7b, 0x8e, 0x20, 0xa3, 0x1c};

// CFirewallInstallerApp::InitInstance - DLL initialization

BOOL CFirewallInstallerApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CFirewallInstallerApp::ExitInstance - DLL termination

int CFirewallInstallerApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED( CreateComponentCategory(CATID_SafeForScripting, L"Controls that are safely scriptable") ))
                return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data") ))
            return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( RegisterCLSIDInCategory(_ctlid, CATID_SafeForScripting) ))
            return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( RegisterCLSIDInCategory(_ctlid, CATID_SafeForInitializing) ))
            return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
