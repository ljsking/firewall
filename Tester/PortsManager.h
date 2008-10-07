#pragma once

class PortsManager
{
public:
	PortsManager();
	void Init(FilterHelper *helper, CListCtrl *list);
	void Update();
private:
	PortSet GetNowPorts();
	PortSet ports;
	FilterHelper *m_helper;
	CListCtrl *m_list;
};