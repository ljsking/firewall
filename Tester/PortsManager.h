#pragma once

class PortsManager
{
public:
	PortsManager();
	void Init(FilterHelper *helper, CListCtrl *list, CChartCtrl *chart);
	void Update();
	void ChangeSelected(int id);
private:
	int GetPortFromList(int id);
	PortSet GetNowPorts();
	bool UpdatePort(PortSet::iterator &iter);
	void UpdateChart();
	PortSet m_ports;
	FilterHelper *m_helper;
	CListCtrl *m_list;
	CChartCtrl *m_chart;
	TraceSet m_traces;
	int m_selectedPort;
};