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

#include "stdafx.h"
#include "ChartScrollBar.h"
#include "ChartAxis.h"
#include "ChartCtrl.h"
#include "math.h"

CChartScrollBar::CChartScrollBar(CChartAxis* pParentAxis) 
 : CScrollBar(), m_pParentAxis(pParentAxis), m_bEnabled(false),
   m_bAutoHide(true)
{
}

CChartScrollBar::~CChartScrollBar()
{
}

void CChartScrollBar::CreateScrollBar(const CRect& PlottingRect)
{
	CRect Temp = PlottingRect;
	Temp.top++; Temp.left++;

	DWORD dwStyle = SBS_HORZ | WS_CHILD;
	if (m_pParentAxis->IsHorizontal())
	{
		if (m_pParentAxis->m_bIsSecondary)
			dwStyle |= SBS_TOPALIGN;
		else
			dwStyle += SBS_BOTTOMALIGN;
	}
	else
	{
		if (m_pParentAxis->m_bIsSecondary)
			dwStyle |= SBS_VERT | SBS_RIGHTALIGN;
		else
			dwStyle += SBS_VERT | SBS_LEFTALIGN;
	}
	CScrollBar::Create(dwStyle, Temp, m_pParentAxis->m_pParent,100);
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);	
	info.fMask = SIF_ALL;     
	info.nMin = 1;     
	info.nMax = 1; 
	info.nPage = 1; 
	info.nPos = 1;   
	CScrollBar::SetScrollInfo(&info);

}

bool CChartScrollBar::IsScrollInverted() const
{
	bool bInverted = false;
	if (m_pParentAxis->IsInverted() && m_pParentAxis->m_bIsHorizontal)
		bInverted = true;
	if (!m_pParentAxis->IsInverted() && !m_pParentAxis->m_bIsHorizontal)
		bInverted = true;

	return bInverted;
}

void CChartScrollBar::Refresh()
{
	double AxisMin=0, AxisMax=0;
	double SeriesMin=0, SeriesMax=0;
	m_pParentAxis->GetMinMax(AxisMin,AxisMax);
	m_pParentAxis->GetSeriesMinMax(SeriesMin,SeriesMax);

	double dStep = 0;
	int iTotalSteps = 0;
	int iCurrentStep = 0;
	if (m_pParentAxis->IsLogarithmic())
	{
		// TODO: do something if the series has 0 in it
		dStep = pow(AxisMax/AxisMin,0.1);
		iTotalSteps = (int)ceil(log(SeriesMax/SeriesMin)/log(dStep));
		iCurrentStep = (int)(log(AxisMin/SeriesMin)/log(dStep)); 
	}
	else
	{
		dStep = (AxisMax - AxisMin) / 10.0;
		iTotalSteps = (int)ceil((SeriesMax - SeriesMin)/dStep);
		iCurrentStep = (int)(iTotalSteps * ((AxisMin - SeriesMin)/(SeriesMax-SeriesMin)));
	}

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);	
	info.fMask = SIF_ALL;     

	if ( (AxisMax-AxisMin) == 0 || (SeriesMax-SeriesMin)==0 )
	{
		info.nMin = 1;     
		info.nMax = 1; 
		info.nPage = 1; 
		info.nPos = 1;   
	}
	else
	{
		info.nMin = 1;     
		info.nMax = iTotalSteps; 
		info.nPage = 10; 
		info.nPos = iCurrentStep;   

		if (IsScrollInverted())
			info.nPos = iTotalSteps - 9 - iCurrentStep; 
		else
			info.nPos = iCurrentStep;   
	}
	CScrollBar::SetScrollInfo(&info);
}

void CChartScrollBar::OnHScroll(UINT nSBCode, UINT nPos)
{
	int MinPos;
	int MaxPos;
	int PreviousPos = CScrollBar::GetScrollPos();
	CScrollBar::GetScrollRange(&MinPos, &MaxPos); 
	int CurPos = PreviousPos;

	bool bUpdate = true;
	switch (nSBCode)
	{
	case SB_LEFT:      
		CurPos = 0;
		break;
	case SB_RIGHT:      
		CurPos = MaxPos;
		break;
	case SB_ENDSCROLL:  
		bUpdate = false;
		break;
	case SB_LINELEFT:  
		if (CurPos > MinPos)
			CurPos--;
		break;
	case SB_LINERIGHT:   
		if (CurPos < MaxPos-9)
			CurPos++;
		break;
	case SB_PAGELEFT:    
		if (CurPos > MinPos)
			CurPos = max(MinPos, CurPos - 10);
		break;
	case SB_PAGERIGHT:     
		if (CurPos < MaxPos-9)
			CurPos = min(MaxPos, CurPos + 10);
		break;
	case SB_THUMBPOSITION: 
			CurPos = nPos;
		break;
	case SB_THUMBTRACK:   
			CurPos = nPos;
		break;
	}

	if (bUpdate)
	{
		// Set the new position of the thumb (scroll box).
		CScrollBar::SetScrollPos(CurPos);
		MoveAxisToPos(PreviousPos,CurPos);
	}
}

void CChartScrollBar::OnVScroll(UINT nSBCode, UINT nPos)
{
	int MinPos;
	int MaxPos;
	int PreviousPos = CScrollBar::GetScrollPos();
	CScrollBar::GetScrollRange(&MinPos, &MaxPos); 
	int CurPos = PreviousPos;
	bool bUpdate = true;

	switch (nSBCode)
	{
	case SB_BOTTOM:      
		CurPos = MaxPos;
		break;
	case SB_TOP:      
		CurPos = 0;
		break;
	case SB_ENDSCROLL:   
		bUpdate = false;
		break;
	case SB_LINEDOWN:  
		if (CurPos < MaxPos-9)
			CurPos++;
		break;
	case SB_LINEUP:   
		if (CurPos > MinPos)
			CurPos--;
		break;
	case SB_PAGEUP:    
		if (CurPos > MinPos)
			CurPos = max(MinPos, CurPos - 10);
		break;
	case SB_PAGEDOWN:     
		if (CurPos < MaxPos-9)
			CurPos = min(MaxPos, CurPos + 10);
		break;
	case SB_THUMBPOSITION: 
		CurPos = nPos;
		break;
	case SB_THUMBTRACK:   
		CurPos = nPos;
		break;
	}

	if (bUpdate)
	{
		// Set the new position of the thumb (scroll box).
		CScrollBar::SetScrollPos(CurPos);
		MoveAxisToPos(PreviousPos,CurPos);
	}
}

void CChartScrollBar::MoveAxisToPos(int PreviousPos, int CurPos)
{
	double AxisMin=0, AxisMax=0;
	double SeriesMin=0, SeriesMax=0;
	m_pParentAxis->GetMinMax(AxisMin,AxisMax);
	m_pParentAxis->GetSeriesMinMax(SeriesMin,SeriesMax);

	if (m_pParentAxis->IsLogarithmic())
	{
		double dStep = pow(AxisMax/AxisMin,0.1);
		double dFactor = pow(dStep,(CurPos - PreviousPos));
		if (IsScrollInverted())
			m_pParentAxis->SetZoomMinMax(AxisMin/dFactor,AxisMax/dFactor);
		else
			m_pParentAxis->SetZoomMinMax(AxisMin*dFactor,AxisMax*dFactor);
	}
	else
	{
		double dStep = (AxisMax - AxisMin) / 10.0;
		double dOffset = (CurPos - PreviousPos) * dStep;
		if (IsScrollInverted())
			m_pParentAxis->SetZoomMinMax(AxisMin-dOffset,AxisMax-dOffset);
		else
			m_pParentAxis->SetZoomMinMax(AxisMin+dOffset,AxisMax+dOffset);
	}
}

void CChartScrollBar::OnMouseEnter()
{
	if (m_bEnabled && m_bAutoHide)
		ShowWindow(SW_SHOW);
}

void CChartScrollBar::OnMouseLeave()
{
	if (m_bEnabled && m_bAutoHide)
		ShowWindow(SW_HIDE);
}
