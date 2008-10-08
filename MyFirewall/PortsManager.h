#pragma once

class PortsManager
{
public:
	PortsManager();
	void Init(DriverHelper *helper, CListCtrl *list, CChartCtrl *chart);
	void Update();
	void ChangeSelected(int id);
	int	SessionCount()const;
	int GetTotal()const;
private:
	int GetPortFromList(int id);
	PortSet GetNowPorts();
	bool UpdatePort(PortSet::iterator &iter);
	void UpdateChart();
	PortSet m_ports;
	DriverHelper *m_helper;
	CListCtrl *m_list;
	CChartCtrl *m_chart;
	TraceSet m_traces;
	int m_selectedPort;
	ULONG m_totalPacket;
};