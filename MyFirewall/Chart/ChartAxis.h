/*
 *
 *	ChartAxis.h
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

#if !defined(AFX_CHARTAXIS_H__063D695C_43CF_4A46_8AA0_C7E00268E0D3__INCLUDED_)
#define AFX_CHARTAXIS_H__063D695C_43CF_4A46_8AA0_C7E00268E0D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"
#include "ChartScrollBar.h"
#include "ChartString.h"
#include <afx.h>

#include <list>

class CChartGrid;
class CChartSerie;
class CChartAxisLabel;
    
class CChartAxis : public CChartObject  
{
	friend CChartCtrl;
	friend CChartGrid;
	friend CChartSerie;
	friend CChartScrollBar;

public:

	enum AxisType
	{
		atStandard = 0,
		atLogarithmic,
		atDateTime
	};
	void	 SetAxisType(AxisType Type);
	AxisType GetAxisType() const		 { return m_AxisType; }

	enum TimeInterval
	{
		tiSecond,
		tiMinute,
		tiHour,
		tiDay,
		tiMonth,
		tiYear
	};
	void   SetDateTimeIncrement(bool bAuto, TimeInterval Interval, int Multiplier);
	void   SetTickIncrement(bool bAuto, double Increment);
	double GetTickIncrement() const  { return m_TickIncrement; }

	void SetDateTimeFormat(bool bAutomatic, const TChartString& strFormat);

	int GetPosition();

	void SetInverted(bool bNewValue);
	bool IsInverted() const  { return m_bIsInverted; }
	void SetLogarithmic(bool bNewValue) 
	{ 
		if (bNewValue)
			m_AxisType = atLogarithmic; 
		else
			m_AxisType = atStandard;
		RefreshAutoAxis();
	}
	bool IsLogarithmic() const          { return (m_AxisType == atLogarithmic); }
	void SetAutomatic(bool bNewValue);  
	bool IsAutomatic()  const  { return m_bIsAutomatic; }

	void SetMinMax(double Minimum, double Maximum);
	void GetMinMax(double& Minimum, double& Maximum) const
	{
		Minimum = m_MinValue;
		Maximum = m_MaxValue;
	}

    long   ValueToScreen(double Value) const;
    double ScreenToValue(long ScreenVal) const;
  
	void	 SetTextColor(COLORREF NewColor);
	COLORREF GetTextColor() const			  { return m_TextColor;		}

	void SetFont(int nPointSize, const TChartString& strFaceName);

	CChartAxisLabel* GetLabel() const  { return m_pAxisLabel; }
	CChartGrid*		 GetGrid()	const  { return m_pAxisGrid;  }

	CChartAxis(CChartCtrl* pParent,bool bHoriz);
	virtual ~CChartAxis();

	void SetMarginSize(bool bAuto, int iNewSize);

	void SetPanZoomEnabled(bool bEnabled) { m_bZoomEnabled = bEnabled; }
	void SetZoomLimit(double dLimit)   { m_dZoomLimit = dLimit; }

	void EnableScrollBar(bool bEnabled);
	bool ScrollBarEnabled() const  
	{ 
		if (m_pScrollBar)
			return (m_pScrollBar->GetEnabled()); 
		else
			return false;
	}
	void SetAutoHideScrollBar(bool bAutoHide);
	bool GetAutoHideScrollBar() const;

private:
	void CalculateTickIncrement();
	void CalculateFirstTick();
	double GetNextTickValue(double Previous);
	CString GetTickLabel(double Tick);
	CSize GetLargestTick(CDC* pDC);
	void Recalculate();
	COleDateTime AddMonthToDate(const COleDateTime& Date, int iMonthsToAdd);
	void DrawLabel(CDC* pDC);
	void RefreshDTTickFormat();

	void PanAxis(long PanStart, long PanEnd);
	void SetZoomMinMax(double Minimum, double Maximum);
	void UndoZoom();

	void SetDecimals(int NewValue)  { m_DecCount = NewValue; }
	bool IsHorizontal() const  { return m_bIsHorizontal; }

    int  GetAxisLenght() const;
	void SetSecondary(bool bNewVal)  { m_bIsSecondary = bNewVal; }
	bool GetSecondary() const		 { return m_bIsSecondary; }

	bool RefreshAutoAxis();
	void GetSeriesMinMax(double& Minimum, double& Maximum);

	void SetAxisSize(CRect ControlRect,CRect MarginRect);
	int ClipMargin(CRect ControlRect,CRect& MarginRect,CDC* pDC);	// Allows to calculate the margin required to displayys ticks and text

	void Draw(CDC* pDC);

	// To register/Unregister series related to this axis
	void RegisterSeries(CChartSerie* pSeries);
	void UnregisterSeries(CChartSerie* pSeries);

	void CreateScrollBar();
	void UpdateScrollBarPos();
	void RefreshScrollBar();

	AxisType m_AxisType;		// Type of the axis (standard, log or date/time)

	bool m_bIsHorizontal;	  // Indicates if this is an horizontal or vertical axis
	bool m_bIsInverted;		  // Indicates if the axis is inverted
	bool m_bIsAutomatic;      // Indicates if the axis is automatic

	bool m_bIsSecondary;	// If the axis is secondary, it will be positioned to 
                            // the right (vertical) or to the top (horizontal)
	double m_MaxValue;		// Maximum value on the axis
	double m_MinValue;		
	double m_UnzoomMin;		// Min and max values of the axis before it has been zoomed
	double m_UnzoomMax;		// (used when we unzoom the chart -> go back to previous state)

	bool   m_bAutoTicks;		// Specify if the tick increment is manual or automatic
	double m_TickIncrement;		// Indicates the space between ticks (in axis value) or for log axis the mult base between two ticks
	double m_FirstTickVal;

	unsigned int m_DecCount;	// Number of decimals to display
	int m_StartPos;			    // Start position of the axis 
	int m_EndPos;

	int  m_nFontSize;			
	TChartString m_strFontName;

	CChartGrid*			m_pAxisGrid;
	CChartAxisLabel*	m_pAxisLabel;

	typedef std::list<CChartSerie*> SeriesList;
	SeriesList m_pRelatedSeries;		// List containing pointers to series related to this axis

	// The user can specify the size of the margin, instead of
	// having it calculated automatically
	bool m_bAutoMargin;
	int m_iMarginSize;

	COLORREF m_TextColor;

	// Data for the date/time axis type.
	TChartString m_strDTTickFormat;		// Format of the date/time tick labels
	bool m_bAutoTickFormat;
	TimeInterval m_BaseInterval;
	int m_iDTTickIntervalMult;

	bool m_bZoomEnabled;
	double m_dZoomLimit;

	CChartScrollBar* m_pScrollBar;
};

#endif // !defined(AFX_CHARTAXIS_H__063D695C_43CF_4A46_8AA0_C7E00268E0D3__INCLUDED_)
