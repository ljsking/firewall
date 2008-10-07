#include "stdafx.h"
#include <afxcmn.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "port.h"
#include "filterhelper.h"
#include "portsmanager.h"

PortsManager::PortsManager()
{
}
void PortsManager::Init(FilterHelper *helper, CListCtrl *list)
{
	m_helper = helper;
	m_list = list;
}

PortSet PortsManager::GetNowPorts()
{
	PortSet newPorts;

	PMIB_TCPTABLE pTcpTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	char *addr_ptr;
	unsigned short *port_ptr;
	DWORD i;

	/* Get size required by GetTcpTable() */
	if (GetTcpTable(NULL, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
		pTcpTable = (MIB_TCPTABLE *) malloc (dwSize);
	}

	/* Get actual data using GetTcpTable() */
	if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, 0)) == NO_ERROR) {
		if (pTcpTable->dwNumEntries > 0) {
			for (i=0; i<pTcpTable->dwNumEntries; i++) {
				//addr_ptr = (char *)&pTcpTable->table[i].dwLocalAddr;
				port_ptr = (unsigned short *)&pTcpTable->table[i].dwLocalPort;
				//addr_ptr = (char *)&pTcpTable->table[i].dwRemoteAddr;
				//port_ptr = (unsigned short *)&pTcpTable->table[i].dwRemotePort;
				DWORD state = pTcpTable->table[i].dwState;
				TCHAR buf[100];
				wsprintf(buf, _T("%ld"), *port_ptr);
				m_list->InsertItem(1, buf);
				wsprintf(buf, _T("%ld"), 0);
				m_list->SetItem(1, 1, LVIF_TEXT, buf, 0, 0, 0, 0, 0);
				wsprintf(buf, _T("%ld"), state);
				m_list->SetItem(1, 2, LVIF_TEXT, buf, 0, 0, 0, 0, 0);
				newPorts.insert(Port(*port_ptr, state));
			}
		}
	}
	free(pTcpTable);
	return newPorts;
}

void PortsManager::Update()
{
	
	PortSet newPorts = GetNowPorts();

	Ports tmp(ports.size()+newPorts.size());
	PortsIterator end=std::set_difference (ports.begin(), ports.end(), newPorts.begin(), newPorts.end(), tmp.begin());
	TRACE("diff ports %d, %d\n", ports.size(), newPorts.size());
	for(PortsIterator it = tmp.begin();it!=end;it++)
	{
		TRACE("%d\n",it->GetPort());
	}
	ports = newPorts;
}