#pragma once

// FirewallInstallerPropPage.h : Declaration of the CFirewallInstallerPropPage property page class.


// CFirewallInstallerPropPage : See FirewallInstallerPropPage.cpp for implementation.

class CFirewallInstallerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CFirewallInstallerPropPage)
	DECLARE_OLECREATE_EX(CFirewallInstallerPropPage)

// Constructor
public:
	CFirewallInstallerPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_FIREWALLINSTALLER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

