#include "stdafx.h"
#include <afxcmn.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "port.h"
#include "filterhelper.h"
#include "filterhelper.h"
#include "..\\myDriver\\Filter.h"
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
	for(PortsIterator it = tmp.begin();it!=end;it++)
	{
		USHORT port = it->GetPort();
		m_helper->WriteIo(DEL_PORT, &port, sizeof(port));
	}
	end=std::set_difference (newPorts.begin(), newPorts.end(), ports.begin(), ports.end(), tmp.begin());
	ports = newPorts;
	for(PortSet::iterator iter = ports.begin(); iter!=ports.end(); iter++){
		UpdatePort(iter);
	}
	for(int i = 0; i<m_list->GetItemCount(); i++)
	{
		TCHAR szBuffer[1024];
		char ascii[1024];
		DWORD cchBuf(1024);
		LVITEM lvi;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_list->GetItem(&lvi);
		wcstombs(ascii, szBuffer, 1024);
		int port = atoi(ascii);
		PortSet::iterator iter = ports.find(Port(port, 0));
		if(iter!=ports.end())
		{
			wsprintf(szBuffer, _T("%ld"), iter->GetUsage());
			lvi.iItem = i;
			lvi.iSubItem = 1;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = szBuffer;
			m_list->SetItem(&lvi);
			wsprintf(szBuffer, _T("%ld"), iter->GetState());
			lvi.iItem = i;
			lvi.iSubItem = 2;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = szBuffer;
			m_list->SetItem(&lvi);
		}
		else
		{
			m_list->DeleteItem(i);
			i--;
		}
	}
	for(PortsIterator it = tmp.begin();it!=end;it++)
	{
		int i = m_list->GetItemCount();
		TCHAR szBuffer[1024];
		DWORD cchBuf(1024);
		LVITEM lvi;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		wsprintf(szBuffer, _T("%ld"), it->GetPort());
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_list->InsertItem(&lvi);
		lvi.iItem = i;
		lvi.iSubItem = 1;
		lvi.mask = LVIF_TEXT;
		wsprintf(szBuffer, _T("%ld"), it->GetUsage());
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_list->InsertItem(&lvi);
		lvi.iItem = i;
		lvi.iSubItem = 2;
		lvi.mask = LVIF_TEXT;
		wsprintf(szBuffer, _T("%ld"), it->GetState());
		lvi.pszText = szBuffer;
		lvi.cchTextMax = cchBuf;
		m_list->InsertItem(&lvi);
	}
	/*
	*/
}

bool PortsManager::UpdatePort(PortSet::iterator &iter)
{
	//m_helper.RawIo(DWORD code, PVOID inBuffer, DWORD inCount, PVOID outBuffer, DWORD outCount);
	ULONG usage;
	USHORT port = iter->GetPort();
	m_helper->RawIo(GET_PORTUSAGE, &port, sizeof(port), &usage, sizeof(usage));
	iter->SetUsage(usage);
	return true;
}
