// FirewallInstaller.cpp : Implementation of CFirewallInstallerApp and DLL registration.

#include "stdafx.h"
#include "FirewallInstaller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CFirewallInstallerApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x2ADA0098, 0x3A05, 0x443F, { 0xB3, 0x5A, 0xFA, 0xDF, 0xEB, 0xFB, 0x72, 0x81 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



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
