/*
 *
 *	ChartAxis.cpp
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
 *	History:
 *		- 16/05/2006: Bug fix in ScreenToValue function
 *		- 18/05/2006: Added support for panning
 *		- 11/08/2006: Changes done for the automatic axis (Un/RegisterSeries, ...)
 *		- 12/08/2006: Tick increment can be set manually also 
 *		- 08/01/2008: Bug fix in how the logarithmic labels are displayed (trailing 0)
 *		- 08/02/2008: Added first version of date/time axis.
 *		- 15/02/2008: Removed compiler warnings for VC2005.
 *		- 18/02/2008: Added the possibility to disable the zoom for a specific axis and
 *					  set a limit on the zoom.
 *		- 21/02/2008: Fixed a bug for automatic date/time axis.
 *		- 21/02/2008: Fixed a bug, the axis label weren't displayed anymore.
 *		- 29/02/2008: Automatic axis will react when points are suppressed too and we avoid
 *					  multiple refresh if several axes are updated (thanks to Bruno Lavier).
 *		- 21/03/2008: Added a scrollbar.
 *		- 01/08/2008: Bug fix, tick labels for log axis were not always correct: rounding 
 *					  error (thanks to Andrej Ritter).
 *
 */

//#define _WIN32_COM

#include "stdafx.h"
#include "ChartAxis.h"
#include "ChartAxisLabel.h"
#include "ChartGrid.h"
#include "ChartCtrl.h"

#include "Math.h"
#include <sstream>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartAxis::CChartAxis(CChartCtrl* pParent,bool bHoriz):CChartObject(pParent)
{
	m_AxisType = atStandard;

	m_bIsHorizontal = bHoriz;
	m_bIsInverted = false;
	m_bIsAutomatic = false;

	m_bIsSecondary = false;

	m_MaxValue = m_UnzoomMax = 10;
	m_MinValue = m_UnzoomMin = 0;

	m_bAutoTicks = true;
	m_TickIncrement = 1;
	m_FirstTickVal = 0;
	m_DecCount = 0;

	m_StartPos = m_EndPos = 0;

	m_nFontSize = 80;
	m_strFontName = _T("Microsoft Sans Serif");

	m_pAxisGrid = new CChartGrid(pParent,bHoriz);
	m_pAxisLabel = new CChartAxisLabel(pParent,bHoriz);

	m_bAutoMargin = true;
	m_iMarginSize = 0;

	m_TextColor = m_ObjectColor;
	m_strDTTickFormat = _T("%d %b");
	m_bAutoTickFormat = true;
	m_BaseInterval = tiDay;
	m_iDTTickIntervalMult = 1;

	m_bZoomEnabled = true;
	m_dZoomLimit = 0.001;

	m_pScrollBar = NULL;
}

CChartAxis::~CChartAxis()
{
	if (m_pAxisGrid)
	{
		delete m_pAxisGrid;
		m_pAxisGrid = NULL;
	}
	if (m_pAxisLabel)
	{
		delete m_pAxisLabel;
		m_pAxisLabel = NULL;
	}

	if (m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}	

void CChartAxis::SetAxisType(AxisType Type)  
{ 
	m_AxisType = Type; 
	m_pParent->RefreshCtrl();
}

int CChartAxis::ClipMargin(CRect ControlRect,CRect& MarginRect,CDC* pDC)
{
	if (!m_bIsVisible)
		return 0;

	int Size = 0;	

	CSize TickSize = GetLargestTick(pDC);
	CSize LabelSize = m_pAxisLabel->GetSize(pDC);

	if (m_bIsHorizontal)
	{
		if (!m_bAutoMargin)
			Size = m_iMarginSize;
		else
		{
			Size += 4 + 2;		//Space above and under the text

			Size += TickSize.cy;
			Size += LabelSize.cy;

			m_iMarginSize = Size;
		}

		if (!m_bIsSecondary)
		{
			ControlRect.bottom -= Size;
			ControlRect.right -= TickSize.cx/2+3;

			if (ControlRect.bottom < MarginRect.bottom)
				MarginRect.bottom = ControlRect.bottom;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}
		else
		{
			ControlRect.top += Size;
			ControlRect.right -= TickSize.cx/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}

	}
	else
	{
		if (!m_bAutoMargin)
			Size = m_iMarginSize;
		else
		{
			Size += 7 + 1;		//Space before and after the text + Tick

			Size += TickSize.cx;
			Size += LabelSize.cx + 2;
			m_iMarginSize = Size;
		}

		if (!m_bIsSecondary)
		{
			ControlRect.left += Size;
			ControlRect.top += TickSize.cy/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.left > MarginRect.left)
				MarginRect.left = ControlRect.left;
		}
		else
		{
			ControlRect.right -= Size;
			ControlRect.top += TickSize.cy/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}
	}

	return Size;
}

int CChartAxis::GetPosition()
{
	if (m_bIsHorizontal)
	{
		if (m_bIsSecondary)
			return 0;
		else
			return 100;
	}
	else
	{
		if (m_bIsSecondary)
			return 100;
		else
			return 0;
	}
}

void CChartAxis::SetAutomatic(bool bNewValue)  
{ 
	m_bIsAutomatic = bNewValue; 
	if (m_bIsAutomatic)
		m_MinValue = m_MaxValue = 0;

	if (RefreshAutoAxis())
		m_pParent->RefreshCtrl();
}

void CChartAxis::SetTickIncrement(bool bAuto, double Increment)
{
	if (m_AxisType == atDateTime)
		return;

	m_bAutoTicks = bAuto;
	if (!m_bAutoTicks)
		m_TickIncrement = Increment;
	else
		CalculateTickIncrement();

	CalculateFirstTick();
	m_pParent->RefreshCtrl();
}

void CChartAxis::SetDateTimeIncrement(bool bAuto, TimeInterval Interval, int Multiplier)
{
	if (m_AxisType != atDateTime)
		return;

	m_bAutoTicks = bAuto;
	if (!m_bAutoTicks)
	{
		m_BaseInterval = Interval;
		m_iDTTickIntervalMult = Multiplier;
	}
}

void CChartAxis::SetDateTimeFormat(bool bAutomatic, const TChartString& strFormat)
{
	m_bAutoTickFormat = bAutomatic;
	m_strDTTickFormat = strFormat;
	m_pParent->RefreshCtrl();
}

void CChartAxis::SetAxisSize(CRect ControlRect,CRect MarginRect)
{
	if (m_bIsHorizontal)
	{
		m_StartPos = MarginRect.left;	
		m_EndPos = MarginRect.right;

		if (!m_bIsSecondary)
		{
			CRect AxisSize = ControlRect;
			AxisSize.top = MarginRect.bottom;
			SetRect(AxisSize);	
		}
		else
		{
			CRect AxisSize = ControlRect;
			AxisSize.bottom = MarginRect.top;
			SetRect(AxisSize);	
		}
	}
	else
	{
		m_StartPos = MarginRect.bottom;
		m_EndPos = MarginRect.top;

		if (!m_bIsSecondary)
		{
			CRect AxisSize = ControlRect;
			AxisSize.right = MarginRect.left;
			SetRect(AxisSize);
		}
		else
		{
			CRect AxisSize = ControlRect;
			AxisSize.left = MarginRect.right;
			SetRect(AxisSize);
		}
	}
}

void CChartAxis::Recalculate()
{
	CalculateTickIncrement();
	CalculateFirstTick();
}

void CChartAxis::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc() == NULL)
		return;

	CPen SolidPen(PS_SOLID,0,m_ObjectColor);
	CPen* pOldPen;

	CFont NewFont;
	CFont* pOldFont;
	COLORREF OldTextColor;
	NewFont.CreatePointFont(m_nFontSize,m_strFontName.c_str(),pDC) ;
	pOldPen = pDC->SelectObject(&SolidPen);
	pOldFont = pDC->SelectObject(&NewFont);
	OldTextColor = pDC->SetTextColor(m_TextColor);
	int iPrevMode = pDC->SetBkMode(TRANSPARENT);

	CSize LabelSize = m_pAxisLabel->GetSize(pDC);

	// Draw the axis line 
	int Pos = 0;
	if (m_bIsHorizontal)
	{
		if (!m_bIsSecondary)
			Pos = m_ObjectRect.top+1;
		else
			Pos = m_ObjectRect.bottom-1;
		pDC->MoveTo(m_StartPos,Pos);
		pDC->LineTo(m_EndPos,Pos);
	}
	else
	{
		if (!m_bIsSecondary)
			Pos = m_ObjectRect.right-1;
		else
			Pos = m_ObjectRect.left+1;
		pDC->MoveTo(Pos,m_StartPos);
		pDC->LineTo(Pos,m_EndPos);
	}

	// Draw the label
	DrawLabel(pDC);
	m_pAxisGrid->ClearTicks();

	//char szBuffer[255];
	CString strBuffer;
	int TickPos = 0;
    double TickValue = m_FirstTickVal;
	do 
	{
		strBuffer = GetTickLabel(TickValue);
		CSize TextSize = pDC->GetTextExtent(strBuffer);

		TickPos = ValueToScreen(TickValue);

		if (m_bIsHorizontal)
		{
			if (!m_bIsSecondary)
			{
				pDC->MoveTo(TickPos,m_ObjectRect.top+1);
				pDC->LineTo(TickPos,m_ObjectRect.top+4);
				pDC->ExtTextOut(TickPos-TextSize.cx/2,m_ObjectRect.top+5,
								ETO_CLIPPED|ETO_OPAQUE,NULL,strBuffer,NULL);
			}
			else
			{
				pDC->MoveTo(TickPos,m_ObjectRect.bottom-1);
				pDC->LineTo(TickPos,m_ObjectRect.bottom-4);
				pDC->ExtTextOut(TickPos-TextSize.cx/2,m_ObjectRect.bottom-5-TextSize.cy,
								ETO_CLIPPED|ETO_OPAQUE,NULL,strBuffer,NULL);
			}
		}
		else
		{
			if (!m_bIsSecondary)
			{
				pDC->MoveTo(m_ObjectRect.right-1,TickPos);
				pDC->LineTo(m_ObjectRect.right-4,TickPos);
				pDC->ExtTextOut(m_ObjectRect.left+LabelSize.cx+4,TickPos-TextSize.cy/2,
								ETO_CLIPPED|ETO_OPAQUE,NULL,strBuffer,NULL);
			}
			else
			{
				pDC->MoveTo(m_ObjectRect.left+1,TickPos);
				pDC->LineTo(m_ObjectRect.left+4,TickPos);
				pDC->ExtTextOut(m_ObjectRect.left+6,TickPos-TextSize.cy/2,
								ETO_CLIPPED|ETO_OPAQUE,NULL,strBuffer,NULL);
			}
		}

		m_pAxisGrid->AddTick(TickPos);
		TickValue = GetNextTickValue(TickValue);
	} while ((TickValue < m_MaxValue+0.0000001) && (m_TickIncrement || m_iDTTickIntervalMult) );

	CRect Size = m_pParent->GetPlottingRect();
	m_pAxisGrid->SetRect(Size);
	m_pAxisGrid->Draw(pDC);

	pDC->SelectObject(pOldPen);
	DeleteObject(SolidPen);
	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SetTextColor(OldTextColor);
	pDC->SetBkMode(iPrevMode);
}

void CChartAxis::DrawLabel(CDC* pDC)
{
	// Draw the axis label.
	CSize LabelSize = m_pAxisLabel->GetSize(pDC);
	int HalfAxisPos = (int)fabs((m_EndPos + m_StartPos)/2.0);
	int XPos = 0;
	int YPos = 0;
	if (m_bIsHorizontal)
	{
		if (!m_bIsSecondary)
		{
			CString Buffer;
			Buffer.Format(_T("%.*f"),m_DecCount,m_MaxValue);
			CSize TextSize = pDC->GetTextExtent(Buffer);
			
			YPos = m_ObjectRect.top  + TextSize.cy + 2;
			XPos = HalfAxisPos - LabelSize.cx/2;
		}
		else
		{
			YPos = m_ObjectRect.top  + 0;
			XPos = HalfAxisPos - LabelSize.cx/2;
		}
	}
	else
	{
		if (!m_bIsSecondary)
		{
			YPos = HalfAxisPos + LabelSize.cy/2;
			XPos = m_ObjectRect.left + 0;
		}
		else
		{
			YPos = HalfAxisPos + LabelSize.cy/2;
			XPos = m_ObjectRect.right - LabelSize.cx - 2;
		}
	}
	m_pAxisLabel->SetPosition(XPos,YPos,pDC);
	m_pAxisLabel->Draw(pDC);
}

void CChartAxis::SetMinMax(double Minimum, double Maximum)
{
	if (Minimum > Maximum)
	{
		TRACE("Maximum axis value must be > minimum axis value");
		return;
	}

	m_MinValue = m_UnzoomMin = Minimum;
	m_MaxValue = m_UnzoomMax = Maximum;
	RefreshScrollBar();
	m_pParent->RefreshCtrl();
}


void CChartAxis::SetInverted(bool bNewValue)
{
	m_bIsInverted = bNewValue;
	RefreshScrollBar();
	m_pParent->RefreshCtrl();
}

int CChartAxis::GetAxisLenght() const
{
    int Length = (int)fabs( (m_EndPos-m_StartPos) * 1.0);
    return Length;
}

long CChartAxis::ValueToScreen(double Value) const
{
    long Offset = 0;
	if (m_MaxValue==m_MinValue)
	{
		Offset = (int)fabs((m_EndPos-m_StartPos)/2.0);
		if (m_bIsHorizontal)
			return m_StartPos + Offset;
		else
			return m_StartPos - Offset;
	}

	if (m_AxisType != atLogarithmic)
        Offset = (int)floor( (Value - m_MinValue) * GetAxisLenght()/(m_MaxValue-m_MinValue) );
	else
		Offset = (int)floor((log10(Value)-log10(m_MinValue)) * GetAxisLenght()/(log10(m_MaxValue)-log10(m_MinValue)) );
 
    if (m_bIsHorizontal)
    {
	   if (!m_bIsInverted)
		  return (m_StartPos + Offset);
	   else
		   return (m_EndPos - Offset);
     }
     else
     {
    	if (!m_bIsInverted)
    		return (m_StartPos - Offset);
    	else
    		return (m_EndPos + Offset);
     }
}

double CChartAxis::ScreenToValue(long ScreenVal) const
{
	if (m_MaxValue==m_MinValue)
		return 0;

    int AxisOffset = 0;
    if (!m_bIsHorizontal)
    {
        if (m_bIsInverted)
			AxisOffset = ScreenVal - m_EndPos;      
        else
			AxisOffset = m_StartPos - ScreenVal;
            
    }
    else
    {
        if (!m_bIsInverted)
           AxisOffset = ScreenVal - m_StartPos;
        else
            AxisOffset = m_EndPos - ScreenVal;
    }

	if (m_AxisType != atLogarithmic)
		return ( (AxisOffset * 1.0 / GetAxisLenght()*(m_MaxValue-m_MinValue)) + m_MinValue);
	else
        return (pow(10.0,(AxisOffset *1.0 / GetAxisLenght()*(log10(m_MaxValue)-log10(m_MinValue)) ) + log10(m_MinValue)) );
}
   
void CChartAxis::SetTextColor(COLORREF NewColor) 
{ 
	m_TextColor = NewColor; 
	m_pParent->RefreshCtrl();
}

void CChartAxis::SetFont(int nPointSize, const TChartString&  strFaceName)
{
	m_nFontSize = nPointSize;
	m_strFontName = strFaceName;
	m_pParent->RefreshCtrl();
}

void CChartAxis::SetMarginSize(bool bAuto, int iNewSize)
{
	m_bAutoMargin = bAuto;
	m_iMarginSize = iNewSize;
	m_pParent->RefreshCtrl();
}

void CChartAxis::PanAxis(long PanStart, long PanEnd)
{
	double StartVal = ScreenToValue(PanStart);
	double EndVal = ScreenToValue(PanEnd);

	if (m_AxisType != atLogarithmic)
	{
		double Shift = StartVal - EndVal;
		SetZoomMinMax(m_MinValue+Shift,m_MaxValue+Shift);
	}
	else
	{
		double Factor = StartVal/EndVal;
		SetZoomMinMax(m_MinValue*Factor,m_MaxValue*Factor);
	}
}

void CChartAxis::SetZoomMinMax(double Minimum, double Maximum)
{
	if (!m_bZoomEnabled)
		return;

	if (Minimum > Maximum)
	{
		TRACE("Maximum axis value must be > minimum axis value");
		return;
	}

	m_MinValue = Minimum;
	if ( (Maximum - Minimum) < m_dZoomLimit && m_AxisType!=atLogarithmic)
		m_MaxValue = m_MinValue + m_dZoomLimit;
	else
		m_MaxValue = Maximum;
	RefreshScrollBar();
}

void CChartAxis::UndoZoom()
{
	SetMinMax(m_UnzoomMin,m_UnzoomMax);
}

void CChartAxis::RegisterSeries(CChartSerie* pSeries)
{
	// First check if the series is already present in the list
	SeriesList::iterator iter = m_pRelatedSeries.begin();
	for (iter; iter!=m_pRelatedSeries.end(); iter++)
	{
		if ( (*iter) == pSeries)
			return;
	}

	m_pRelatedSeries.push_back(pSeries);
}

void CChartAxis::UnregisterSeries(CChartSerie* pSeries)
{
	SeriesList::iterator iter = m_pRelatedSeries.begin();
	for (iter; iter!=m_pRelatedSeries.end(); iter++)
	{
		if ( (*iter) == pSeries)
		{
			m_pRelatedSeries.erase(iter);
			return;
		}
	}
}

COleDateTime CChartAxis::AddMonthToDate(const COleDateTime& Date, int iMonthsToAdd)
{
	COleDateTime newDate;
	int nMonths = Date.GetMonth()-1 + iMonthsToAdd;
	int nYear = Date.GetYear() + nMonths/12;

	newDate.SetDateTime(nYear,nMonths%12+1,Date.GetDay(),Date.GetHour(),
		Date.GetMinute(),Date.GetSecond());
	return newDate;
}

bool CChartAxis::RefreshAutoAxis()
{
	RefreshScrollBar();
	bool bNeedRefresh = false;
	if (!m_bIsAutomatic)
		return bNeedRefresh;

	double SeriesMin = 0;
	double SeriesMax = 0;
	GetSeriesMinMax(SeriesMin, SeriesMax);

	if ( (SeriesMax!=m_MaxValue) || (SeriesMin!=m_MinValue) )
		SetMinMax(SeriesMin,SeriesMax);

	return bNeedRefresh;
}

void CChartAxis::GetSeriesMinMax(double& Minimum, double& Maximum)
{
	Minimum = 0;
	Maximum = 0;
	double TempMin = 0;
	double TempMax = 0;
	
	SeriesList::iterator iter = m_pRelatedSeries.begin();
	if (iter != m_pRelatedSeries.end())
	{
		if (m_bIsHorizontal)
			(*iter)->GetSerieXMinMax(Minimum,Maximum);
		else
			(*iter)->GetSerieYMinMax(Minimum,Maximum);
	}

	for (iter; iter!=m_pRelatedSeries.end(); iter++)
	{
		if (m_bIsHorizontal)
			(*iter)->GetSerieXMinMax(TempMin,TempMax);
		else
			(*iter)->GetSerieYMinMax(TempMin,TempMax);

		if (TempMin < Minimum)
			Minimum = TempMin;
		if (TempMax > Maximum)
			Maximum = TempMax;
	}
}

void CChartAxis::CalculateTickIncrement()
{
	if (!m_bAutoTicks)
		return;

	if (m_MaxValue == m_MinValue)
	{
		m_iDTTickIntervalMult = 0;
		m_TickIncrement = 0;
		return;
	}

	int PixelSpace;
	if (m_bIsHorizontal)
	{
		if (m_AxisType == atDateTime)
			PixelSpace = 60;
		else
			PixelSpace = 30;
	}
	else
		PixelSpace = 20;

	int MaxTickNumber = (int)fabs((m_EndPos-m_StartPos)/PixelSpace * 1.0);

	//Calculate the appropriate TickSpace (1 tick every 30 pixel +/-)
	switch (m_AxisType)
	{
	case atLogarithmic:
	   m_TickIncrement = 10;
	   break;

	case atStandard:
		{
	   		//Temporary tick increment
    		double TickIncrement = (m_MaxValue-m_MinValue)/MaxTickNumber;
	    
    		// Calculate appropriate tickSpace (not rounded on 'strange values' but 
    		// on something like 1, 2 or 5*10^X  where X is optimalized for showing the most
    		// significant digits)
    		int Zeros = (int)floor(log10(TickIncrement));
    		double MinTickIncrement = pow(10.0,Zeros);
	    
    		int Digits = 0;
    		if (Zeros<0)		
    		{
				//We must set decimal places. In the other cases, Digits will be 0.
    			Digits = (int)fabs(Zeros*1.0);
    		}
	    
    		if (MinTickIncrement>=TickIncrement)
    		{
    			m_TickIncrement = MinTickIncrement;
    			SetDecimals(Digits);
    		}
    		else if (MinTickIncrement*2>=TickIncrement)
    		{
    			m_TickIncrement = MinTickIncrement*2;
    			SetDecimals(Digits);
    		}
    		else if (MinTickIncrement*5>=TickIncrement)
    		{
    			m_TickIncrement = MinTickIncrement*5;
    			SetDecimals(Digits);
    		}
    		else if (MinTickIncrement*10>=TickIncrement)
    		{
    			m_TickIncrement = MinTickIncrement*10;
    			if (Digits)
    				SetDecimals(Digits-1);
    			else
    				SetDecimals(Digits);
    		}
		}
		break;

	case atDateTime:
		{
			COleDateTime StartDate(m_MinValue);
			COleDateTime EndDate(m_MaxValue);

			COleDateTimeSpan minTickInterval = (EndDate - StartDate)/MaxTickNumber;
			double Seconds = minTickInterval.GetTotalSeconds();
			double Minutes = minTickInterval.GetTotalMinutes();
			double Hours = minTickInterval.GetTotalHours();
			double Days = minTickInterval.GetTotalDays();
			if (Seconds < 60)
			{
				m_BaseInterval = tiSecond;
				if (Seconds > 30)
				{
					m_BaseInterval = tiMinute;
					m_iDTTickIntervalMult = 1;
				}
				else if (Seconds > 10)
					m_iDTTickIntervalMult = 30;
				else if (Seconds > 5)
					m_iDTTickIntervalMult = 10;
				else if (Seconds > 2)
					m_iDTTickIntervalMult = 5;
				else 
					m_iDTTickIntervalMult = 1;
			}
			else if (Minutes < 60)
			{
				m_BaseInterval = tiMinute;
				if (Minutes > 30)
				{
					m_BaseInterval = tiHour;
					m_iDTTickIntervalMult = 1;
				}
				else if (Minutes > 10)
					m_iDTTickIntervalMult = 30;
				else if (Minutes > 5)
					m_iDTTickIntervalMult = 10;
				else if (Minutes > 2)
					m_iDTTickIntervalMult = 5;
				else 
					m_iDTTickIntervalMult = 2;
			}
			else if (Hours < 24)
			{
				m_BaseInterval = tiHour;
				if (Hours > 12)
				{
					m_BaseInterval = tiDay;
					m_iDTTickIntervalMult = 1;
				}
				else if (Hours > 6)
					m_iDTTickIntervalMult = 12;
				else if (Hours > 2)
					m_iDTTickIntervalMult = 6;
				else 
					m_iDTTickIntervalMult = 2;
			}
			else if (Days < 31)
			{
				m_BaseInterval = tiDay;
				if (Days > 7)
				{
					m_BaseInterval = tiMonth;
					m_iDTTickIntervalMult = 1;
				}
				else if (Days > 2)
				{
					m_BaseInterval = tiDay;
					m_iDTTickIntervalMult = 7;
				}
				else 
					m_iDTTickIntervalMult = 2;
			}
			else if (Days < 365)
			{
				m_BaseInterval = tiMonth;
				if (Days > 186)	 // Approx 6 months
				{
					m_BaseInterval = tiYear;
					m_iDTTickIntervalMult = 1;
				}
				else if (Days > 124)
					m_iDTTickIntervalMult = 6;
				else if (Days > 62)
					m_iDTTickIntervalMult = 4;
				else
					m_iDTTickIntervalMult = 2;
			}
			else
			{
				m_BaseInterval = tiYear;
				m_iDTTickIntervalMult = (int)Days/365 + 1;
			}

			if (m_bAutoTickFormat)
				RefreshDTTickFormat();
		}
		break;
	}
}

void CChartAxis::CalculateFirstTick()
{
	switch (m_AxisType)
	{
	case atStandard:
		{
			m_FirstTickVal = 0;
			if (m_TickIncrement!=0)
			{
				if (m_MinValue == 0)
					m_FirstTickVal = 0;
				else if (m_MinValue>0)
				{
					m_FirstTickVal = (int)(m_MinValue/m_TickIncrement) * m_TickIncrement;
					while (m_FirstTickVal<m_MinValue)
						m_FirstTickVal += m_TickIncrement;
				}
				else
				{
					m_FirstTickVal = (int)(m_MinValue/m_TickIncrement) * m_TickIncrement;
					while (m_FirstTickVal>m_MinValue)
						m_FirstTickVal -= m_TickIncrement;
					if (!(m_FirstTickVal == m_MinValue))
						m_FirstTickVal += m_TickIncrement;
				}
			}
			else	// m_TickIncrement!=0
			{
				m_FirstTickVal = m_MinValue;
			}
		}
		break;

	case atLogarithmic:
		{
			int LogBase = (int)log10(m_MinValue);
			m_FirstTickVal = pow(10.0,LogBase);
		}
		break;

	case atDateTime:
		{
			COleDateTime dtMin((DATE)m_MinValue);
			COleDateTime dtFirstTick(dtMin);
			switch (m_BaseInterval)
			{
			case tiSecond:
				dtFirstTick.SetDateTime(dtMin.GetYear(),dtMin.GetMonth(),dtMin.GetDay(),
										dtMin.GetHour(),dtMin.GetMinute(),dtMin.GetSecond()+1);
				break;

			case tiMinute:
				dtFirstTick.SetDateTime(dtMin.GetYear(),dtMin.GetMonth(),dtMin.GetDay(),
										dtMin.GetHour(),dtMin.GetMinute(),0);
				if (dtMin.GetSecond() != 0)
					dtFirstTick += COleDateTimeSpan(0,0,1,0);					
				break;

			case tiHour:
				dtFirstTick.SetDateTime(dtMin.GetYear(),dtMin.GetMonth(),dtMin.GetDay(),
										dtMin.GetHour(),0,0);
				if ( (dtMin.GetMinute()!=0) || (dtMin.GetSecond()!=0) )
					dtFirstTick += COleDateTimeSpan(0,1,0,0);					
				break;

			case tiDay:
				dtFirstTick.SetDate(dtMin.GetYear(),dtMin.GetMonth(),dtMin.GetDay());
				if ( (dtMin.GetHour()!=0) || (dtMin.GetMinute()!=0) || 
					 (dtMin.GetSecond()!=0) )
				{
					dtFirstTick += COleDateTimeSpan(1,0,0,0);
				}
				break;

			case tiMonth:
				{
					dtFirstTick.SetDate(dtMin.GetYear(),dtMin.GetMonth(),1);
					if ((dtMin.GetDay()!=1) || (dtMin.GetHour()!=0) || 
						(dtMin.GetMinute()!=0) || (dtMin.GetSecond()!=0) )
					{
						dtFirstTick = AddMonthToDate(dtFirstTick,1);
					}
				}
				break;

			case tiYear:
				break;
			}
			m_FirstTickVal = (DATE)dtFirstTick;

		}
		break;
	}
}

double CChartAxis::GetNextTickValue(double Previous)
{
	double NewTick = 0;
	switch (m_AxisType)
	{
	case atStandard:
		NewTick = Previous + m_TickIncrement;
		break;

	case atLogarithmic:
		NewTick = Previous * m_TickIncrement;
		break;

	case atDateTime:
		{
			COleDateTime dtTick((DATE)Previous);
			COleDateTimeSpan dtSpan;
			switch (m_BaseInterval)
			{
			case tiSecond:
				dtSpan.SetDateTimeSpan(0,0,0,m_iDTTickIntervalMult);
				dtTick += dtSpan;
				break;
			case tiMinute:
				dtSpan.SetDateTimeSpan(0,0,m_iDTTickIntervalMult,0);
				dtTick += dtSpan;
				break;
			case tiHour:
				dtSpan.SetDateTimeSpan(0,m_iDTTickIntervalMult,0,0);
				dtTick += dtSpan;
				break;
			case tiDay:
				dtSpan.SetDateTimeSpan(m_iDTTickIntervalMult,0,0,0);
				dtTick += dtSpan;
				break;
			case tiMonth:
				{
					dtTick = AddMonthToDate(dtTick,m_iDTTickIntervalMult);
				}
				break;
			case tiYear:
				break;
			}

			NewTick = (DATE)dtTick;
		}
		break;
	}

	return NewTick;
}

CString CChartAxis::GetTickLabel(double TickValue)
{
	CString strLabel;
	switch (m_AxisType)
	{
	case atStandard:
		strLabel.Format(_T("%.*f"),m_DecCount,TickValue);
	//	ssLabel << setprecision(m_DecCount) << TickValue;
	//	sprintf(szBuffer,"%.*f",m_DecCount,TickValue);
		break;

	case atLogarithmic:
		{
			double fLogDecCount; 
			int    nLogDecCount; 

			fLogDecCount = log10(TickValue); 

			if (fLogDecCount < 0.0) 
				nLogDecCount = (int)(fabs(fLogDecCount) + 0.1); 
			else 
				nLogDecCount = 0; 

			strLabel.Format(_T("%.*f"), nLogDecCount, TickValue); 
		}
		break;

	case atDateTime:
		{
			COleDateTime tickTime((DATE)TickValue);
			strLabel = tickTime.Format(m_strDTTickFormat.c_str());

		//	ssLabel << tickTime.Format(m_strDTTickFormat.c_str());
		//	strcpy(szBuffer,strLabel);
		}
		break;
	}

	return strLabel;
}

void CChartAxis::RefreshDTTickFormat()
{
	switch (m_BaseInterval)
	{
	case tiSecond:
		m_strDTTickFormat = _T("%H:%M:%S");
		break;
	case tiMinute:
		m_strDTTickFormat = _T("%H:%M");
		break;
	case tiHour:
		m_strDTTickFormat = _T("%H:00");
		break;
	case tiDay:
		m_strDTTickFormat = _T("%d %b");
		break;
	case tiMonth:
		m_strDTTickFormat = _T("%b %Y");
		break;
	case tiYear:
		m_strDTTickFormat = _T("%Y");
		break;
	}
}

CSize CChartAxis::GetLargestTick(CDC* pDC)
{
	CSize TickSize;

	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_nFontSize,m_strFontName.c_str(),pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	CString strBuffer;
	strBuffer.Format(_T("%.*f"),m_DecCount,m_MaxValue);
	if (m_bIsHorizontal)
		TickSize = pDC->GetTextExtent(strBuffer);
	else
	{
		switch (m_AxisType)
		{
		case atStandard:
			{
				int MaxChars = abs( (int)log10(fabs(m_MaxValue) )) + 1;
    			int MinChars = abs( (int)log10(fabs(m_MinValue) )) + 1;
    			if (m_MinValue<0)
    				MinChars++;
    			if (m_MaxValue<0)
    				MaxChars++;
    			if (MaxChars>MinChars)
					strBuffer.Format(_T("%.*f"),m_DecCount,m_MaxValue);
    			else
					strBuffer.Format(_T("%.*f"),m_DecCount,m_MinValue);
			}
			break;

		case atLogarithmic:
			{
				CString strBuffMax;
				CString strBuffMin;
				int MaxDecCount = (int)log10(m_MaxValue);
				if (MaxDecCount < 0)
					MaxDecCount = -MaxDecCount;
				else
					MaxDecCount = 0;
				strBuffMax.Format(_T("%.*f"),MaxDecCount,m_MaxValue);

				int MinDecCount = (int)log10(m_MinValue);
				if (MinDecCount < 0)
					MinDecCount = -MinDecCount;
				else
					MinDecCount = 0;
				strBuffMin.Format(_T("%.*f"),MinDecCount,m_MinValue);

				if (strBuffMin.GetLength() > strBuffMax.GetLength() )
					strBuffer = strBuffMin;
				else
					strBuffer = strBuffMax;
			}			
			break;

		case atDateTime:
			{
				double TickValue = m_FirstTickVal;
				CString strTemp;
				do 
				{
					strTemp = GetTickLabel(TickValue);
					if (strTemp.GetLength() > strBuffer.GetLength() )
						strBuffer = strTemp;
					TickValue = GetNextTickValue(TickValue);
				} while ((TickValue < m_MaxValue+0.0000001) && (m_TickIncrement|| m_iDTTickIntervalMult) ); 
			}
			break;
		}

 		TickSize = pDC->GetTextExtent(strBuffer);
	}

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	return TickSize;
}

void CChartAxis::EnableScrollBar(bool bEnabled)
{
	if (m_pScrollBar)
	{
		m_pScrollBar->SetEnabled(bEnabled);
		if (bEnabled)
			m_pScrollBar->ShowWindow(SW_SHOW);
		else
			m_pScrollBar->ShowWindow(SW_HIDE);
	}
}

void CChartAxis::SetAutoHideScrollBar(bool bAutoHide)
{
	if (m_pScrollBar)
		m_pScrollBar->SetAutoHide(bAutoHide);
}

bool CChartAxis::GetAutoHideScrollBar() const
{ 
	if (m_pScrollBar)
		return (m_pScrollBar->GetAutoHide()); 
	else
		return false;
}

void CChartAxis::CreateScrollBar()
{
	m_pScrollBar = new CChartScrollBar(this);
	m_pScrollBar->CreateScrollBar(m_pParent->GetPlottingRect());
}

void CChartAxis::UpdateScrollBarPos()
{
	CRect PlottingRect = m_pParent->GetPlottingRect();
	PlottingRect.top++; PlottingRect.left++;

	CRect Temp;
	m_pScrollBar->GetWindowRect(&Temp);
	if (m_bIsHorizontal && !m_bIsSecondary)
		PlottingRect.top = PlottingRect.bottom - Temp.Height();
	if (!m_bIsHorizontal && !m_bIsSecondary)
		PlottingRect.right = PlottingRect.left + Temp.Width();
	if (m_bIsHorizontal && m_bIsSecondary)
		PlottingRect.bottom = PlottingRect.top + Temp.Height();
	if (!m_bIsHorizontal && m_bIsSecondary)
		PlottingRect.left = PlottingRect.right - Temp.Width();

	m_pScrollBar->MoveWindow(&PlottingRect);
}

void CChartAxis::RefreshScrollBar()
{
	if (m_pScrollBar)
		m_pScrollBar->Refresh();
}