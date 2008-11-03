#include "stdafx.h"
#include <afxcmn.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "port.h"
#include "DriverHelper.h"
#include "..\\myDriver\\Filter.h"
#include "Chart\\ChartCtrl.h"
#include "Chart\\ChartLineSerie.h"
#include "usagetrace.h"
#include "portsmanager.h"

PortsManager::PortsManager()
{
}
void PortsManager::Init(DriverHelper *helper, CListCtrl *list, CChartCtrl *chart)
{
	m_helper = helper;
	m_list = list;
	m_chart = chart;
	m_selectedPort = 0;
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
				//port_ptr = (unsigned short *)&pTcpTable->table[i].dwLocalPort;
				//addr_ptr = (char *)&pTcpTable->table[i].dwRemoteAddr;
				port_ptr = (unsigned short *)&pTcpTable->table[i].dwRemotePort;
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
	Ports tmp(m_ports.size()+newPorts.size());
	PortsIterator end=std::set_difference (m_ports.begin(), m_ports.end(), newPorts.begin(), newPorts.end(), tmp.begin());
	for(PortsIterator it = tmp.begin();it!=end;it++)
	{
		USHORT port = it->GetPort();
		//USHORT netPort = htons(port);
		m_helper->WriteIo(DEL_PORT, &port, sizeof(port));
		m_traces.erase(m_traces.find(UsageTrace(port)));
	}
	end=std::set_difference (newPorts.begin(), newPorts.end(), m_ports.begin(), m_ports.end(), tmp.begin());
	m_ports = newPorts;
	for(PortSet::iterator iter = m_ports.begin(); iter!=m_ports.end(); iter++){
		UpdatePort(iter);
	}
	for(int i = 0; i<m_list->GetItemCount(); i++)
	{
		TCHAR szBuffer[1024];
		int port = GetPortFromList(i);
		PortSet::iterator iter = m_ports.find(Port(port, 0));
		if(iter!=m_ports.end())
		{
			wsprintf(szBuffer, _T("%ld"), iter->GetUsage());
			m_list->SetItem(i, 1, LVIF_TEXT, szBuffer, 0, 0, 0, 0);
			switch (iter->GetState()) {
            case MIB_TCP_STATE_CLOSED:
				wsprintf(szBuffer, _T("CLOSED"));
                break;
            case MIB_TCP_STATE_LISTEN:
				wsprintf(szBuffer, _T("LISTEN"));
                break;
            case MIB_TCP_STATE_SYN_SENT:
				wsprintf(szBuffer, _T("SYN-SENT"));
                break;
            case MIB_TCP_STATE_SYN_RCVD:
				wsprintf(szBuffer, _T("SYN-RECEIVED"));
                break;
            case MIB_TCP_STATE_ESTAB:
				wsprintf(szBuffer, _T("ESTABLISHED"));
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
				wsprintf(szBuffer, _T("FIN-WAIT-1"));
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
				wsprintf(szBuffer, _T("FIN-WAIT-2"));
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
				wsprintf(szBuffer, _T("CLOSE-WAIT"));
                break;
            case MIB_TCP_STATE_CLOSING:
				wsprintf(szBuffer, _T("CLOSING"));
                break;
            case MIB_TCP_STATE_LAST_ACK:
				wsprintf(szBuffer, _T("LAST-ACK"));
                break;
            case MIB_TCP_STATE_TIME_WAIT:
				wsprintf(szBuffer, _T("TIME-WAIT"));
                break;
            case MIB_TCP_STATE_DELETE_TCB:
				wsprintf(szBuffer, _T("DELETE-TCB"));
                break;
            default:
				wsprintf(szBuffer, _T("UNKNOWN"));
                break;
            }
		m_list->SetItem(i, 2, LVIF_TEXT, szBuffer, 0, 0, 0, 0);
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
		unsigned short port = ntohs(it->GetPort());
		wsprintf(szBuffer, _T("%u"), port);
		m_list->InsertItem(i, szBuffer);
		wsprintf(szBuffer, _T("%ld"), it->GetUsage());
		m_list->SetItem(i, 1, LVIF_TEXT, szBuffer, 0, 0, 0, 0);
		
		switch (it->GetState()) {
            case MIB_TCP_STATE_CLOSED:
				wsprintf(szBuffer, _T("CLOSED"));
                break;
            case MIB_TCP_STATE_LISTEN:
				wsprintf(szBuffer, _T("LISTEN"));
                break;
            case MIB_TCP_STATE_SYN_SENT:
				wsprintf(szBuffer, _T("SYN-SENT"));
                break;
            case MIB_TCP_STATE_SYN_RCVD:
				wsprintf(szBuffer, _T("SYN-RECEIVED"));
                break;
            case MIB_TCP_STATE_ESTAB:
				wsprintf(szBuffer, _T("ESTABLISHED"));
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
				wsprintf(szBuffer, _T("FIN-WAIT-1"));
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
				wsprintf(szBuffer, _T("FIN-WAIT-2"));
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
				wsprintf(szBuffer, _T("CLOSE-WAIT"));
                break;
            case MIB_TCP_STATE_CLOSING:
				wsprintf(szBuffer, _T("CLOSING"));
                break;
            case MIB_TCP_STATE_LAST_ACK:
				wsprintf(szBuffer, _T("LAST-ACK"));
                break;
            case MIB_TCP_STATE_TIME_WAIT:
				wsprintf(szBuffer, _T("TIME-WAIT"));
                break;
            case MIB_TCP_STATE_DELETE_TCB:
				wsprintf(szBuffer, _T("DELETE-TCB"));
                break;
            default:
				wsprintf(szBuffer, _T("UNKNOWN"));
                break;
            }
		m_list->SetItem(i, 2, LVIF_TEXT, szBuffer, 0, 0, 0, 0);
	}
	UpdateChart();
	m_helper->ReadIo(GET_TOTAL, &m_totalPacket, sizeof(ULONG));
}

bool PortsManager::UpdatePort(PortSet::iterator &iter)
{
	//m_helper.RawIo(DWORD code, PVOID inBuffer, DWORD inCount, PVOID outBuffer, DWORD outCount);
	ULONG usage;
	USHORT port = iter->GetPort();
	//USHORT netPort = htons(port);
	m_helper->RawIo(GET_PORTUSAGE, &port, sizeof(port), &usage, sizeof(usage));
	iter->SetUsage(usage);
	TraceSet::iterator it = m_traces.find(UsageTrace(port));
	if(it == m_traces.end()){
		UsageTrace trace(port);
		trace.InsertUsage(usage);
		m_traces.insert(trace);
	}else{
		it->InsertUsage(usage);
	}
	return true;
}

int PortsManager::GetPortFromList(int id)
{
	TCHAR szBuffer[1024];
	char ascii[1024];
	DWORD cchBuf(1024);
	LVITEM lvi;
	lvi.iItem = id;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szBuffer;
	lvi.cchTextMax = cchBuf;
	m_list->GetItem(&lvi);
	wcstombs(ascii, szBuffer, 1024);
	return htons(atoi(ascii));
}


void PortsManager::ChangeSelected(int id)
{
	int port = GetPortFromList(id);
	m_selectedPort = port;
	UpdateChart();
}

void PortsManager::UpdateChart()
{
	m_chart->RemoveAllSeries();
	CChartLineSerie* pLineSerie = dynamic_cast<CChartLineSerie*> (m_chart->AddSerie(CChartSerie::stLineSerie));
	TraceSet::iterator it = m_traces.find(m_selectedPort);
	TRACE("selected port is %d\n",m_selectedPort);
	if(it == m_traces.end()) return;
	std::list<int>::iterator iter = it->begin();
	int x = 0;
	int y = 0;
	for(;iter!=it->end();iter++){
		pLineSerie->AddPoint(x++, *iter);
		y = *iter;
	}
	m_chart->GetLeftAxis()->SetMinMax(-10,y);
	m_chart->GetBottomAxis()->SetMinMax(-1,x);
}

int PortsManager::SessionCount()const
{
	return m_ports.size();
}

int PortsManager::GetTotal()const
{
	return m_totalPacket;
}