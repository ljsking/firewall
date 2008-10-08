// FirewallInstallerPropPage.cpp : Implementation of the CFirewallInstallerPropPage property page class.

#include "stdafx.h"
#include "FirewallInstaller.h"
#include "FirewallInstallerPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CFirewallInstallerPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CFirewallInstallerPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFirewallInstallerPropPage, "FIREWALLINSTAL.FirewallInstalPropPage.1",
	0xf6bf613b, 0x1aeb, 0x41ec, 0xad, 0x9a, 0x52, 0xbd, 0xba, 0x5f, 0xb0, 0xf3)



// CFirewallInstallerPropPage::CFirewallInstallerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CFirewallInstallerPropPage

BOOL CFirewallInstallerPropPage::CFirewallInstallerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_FIREWALLINSTALLER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CFirewallInstallerPropPage::CFirewallInstallerPropPage - Constructor

CFirewallInstallerPropPage::CFirewallInstallerPropPage() :
	COlePropertyPage(IDD, IDS_FIREWALLINSTALLER_PPG_CAPTION)
{
}



// CFirewallInstallerPropPage::DoDataExchange - Moves data between page and properties

void CFirewallInstallerPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_process);
}



// CFirewallInstallerPropPage message handlers
