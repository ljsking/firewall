#pragma once

// FirewallInstallerCtrl.h : Declaration of the CFirewallInstallerCtrl ActiveX Control class.


// CFirewallInstallerCtrl : See FirewallInstallerCtrl.cpp for implementation.

class CFirewallInstallerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CFirewallInstallerCtrl)

// Constructor
public:
	CFirewallInstallerCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CFirewallInstallerCtrl();

	DECLARE_OLECREATE_EX(CFirewallInstallerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CFirewallInstallerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CFirewallInstallerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CFirewallInstallerCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	};
};

