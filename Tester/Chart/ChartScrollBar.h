/*
 *
 *	ChartScrollBar.h
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#pragma once

class CChartAxis;

class CChartScrollBar : public CScrollBar
{
public:
	CChartScrollBar(CChartAxis* pParentAxis);
	~CChartScrollBar();

	void CreateScrollBar(const CRect& PlottingRect);

	void OnHScroll(UINT nSBCode, UINT nPos);
	void OnVScroll(UINT nSBCode, UINT nPos);
	void Refresh();

	void SetEnabled(bool bEnabled)  { m_bEnabled = bEnabled; }
	bool GetEnabled() const			{ return m_bEnabled; }
	void SetAutoHide(bool bAutoHide)  { m_bAutoHide = bAutoHide; }
	bool GetAutoHide() const		  { return m_bAutoHide; }

	void OnMouseEnter();
	void OnMouseLeave();

private:
	bool IsScrollInverted() const;
	void MoveAxisToPos(int PreviousPos, int CurPos);

	CChartAxis* m_pParentAxis;
	bool m_bEnabled;
	bool m_bAutoHide;
};
