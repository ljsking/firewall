/*
 *
 *	ChartCtrl.h
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


#if !defined(AFX_CHARTCTRL_H__5DDBAE42_77C3_4344_A207_3F39B623343B__INCLUDED_)
#define AFX_CHARTCTRL_H__5DDBAE42_77C3_4344_A207_3F39B623343B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ChartSerie.h"
#include "ChartAxis.h"
#include "ChartGrid.h"
#include "ChartLegend.h"
#include "ChartTitle.h"
#include "ChartGradient.h"
	
#include <vector>



/////////////////////////////////////////////////////////////////////////////
// CChartCtrl window

class CChartCtrl : public CWnd
{

public:
	CDC* GetDC();
	CRect GetPlottingRect()  const { return m_PlottingRect; }

	CChartLegend* GetLegend() const  { return m_pLegend; }
	CChartTitle*  GetTitle()  const  { return m_pTitles; }

	CChartAxis* GetBottomAxis() const;
	CChartAxis* GetLeftAxis() const;
	CChartAxis* GetTopAxis() const;
	CChartAxis* GetRightAxis() const;

	UINT GetEdgeType() const        { return EdgeType;    }
	void SetEdgeType(UINT NewEdge)  
	{ 
		EdgeType = NewEdge; 
		RefreshCtrl();
	}

	COLORREF GetBackColor() const			{ return m_BackColor;   }
	void SetBackColor(COLORREF NewCol)		
	{ 
		m_BackColor = NewCol;  
		m_bBackGradient = false; 
		RefreshCtrl();
	}
	COLORREF GetBorderColor() const			{ return m_BorderColor;   }
	void SetBorderColor(COLORREF NewCol)	{ m_BorderColor = NewCol;	RefreshCtrl(); }
	COLORREF GetZoomRectColor() const		{ return m_ZoomRectColor;   }
	void SetZoomRectColor(COLORREF NewCol)	{ m_ZoomRectColor = NewCol; RefreshCtrl(); }
	void SetBackGradient(COLORREF Col1, COLORREF Col2, EGradientType GradientType);

	CChartSerie* AddSerie(int Type);
	CChartSerie* GetSerie(size_t Index) const;
	void RemoveSerie(size_t Index);
	void RemoveSerie(CChartSerie* pToRemove);
	void RemoveAllSeries();
	size_t GetSeriesCount() const;
	
	void SetPanEnabled(bool bEnabled)  { m_bPanEnabled = bEnabled;  }
	bool GetPanEnabled() const		   { return m_bPanEnabled;	    }
	void SetZoomEnabled(bool bEnabled) { m_bZoomEnabled = bEnabled; }
	bool GetZoomEnabled() const		   { return m_bZoomEnabled;	    }
	void UndoPanZoom();

	void RefreshCtrl();
	void EnableRefresh(bool bEnable);
	int Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle=WS_VISIBLE);
	
	static double DateToValue(const COleDateTime& Date);
	static COleDateTime ValueToDate(double Value);

    void Print(const TChartString& strTitle, CPrintDialog* pPrntDialog = NULL);

    CChartCtrl();	
    virtual ~CChartCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChartCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
    void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

	bool RegisterWindowClass();
	// This function can be called to draw the chart
	// on the screen or for printing.
	void DrawChart(CDC* pDC, CRect ChartRect);

	int m_iEnableRefresh ;
	bool m_bPendingRefresh;
	CDC m_BackgroundDC;		// Contains the background of chart (axes, grid ...)
	bool m_bMemDCCreated;

	bool m_bBackGradient;	// Specifies if the background is gradient or solid
	COLORREF m_BackGradientCol1;
	COLORREF m_BackGradientCol2;
	EGradientType m_BackGradientType;
	COLORREF m_BackColor;	
	COLORREF m_BorderColor;
	UINT EdgeType;		

	CRect m_PlottingRect;	// Zone in wich the series will be plotted

	std::vector<CChartSerie*> m_pSeriesList;		// Table containing all the series (dynamic)
	std::vector<CChartAxis*>  m_pAxisList;			// Table containing all the axes (dynamic)

	CChartLegend* m_pLegend;	// Chart legend
	CChartTitle*  m_pTitles;	// Chart titles

	// Support for mouse panning
	bool m_bPanEnabled;
	bool m_bRMouseDown;		// If the right mouse button is pressed
	CPoint m_PanAnchor;

	// Support for manual zoom
	bool  m_bZoomEnabled;
	bool  m_bLMouseDown;	// If the left mouse button is pressed
	CRect m_rectZoomArea;
	COLORREF m_ZoomRectColor;

	// Specifies if the toolbars have already been created
	bool m_bToolBarCreated;

    CFont  m_PrinterFont;  
    // Printing information
    CSize m_LogicalPageSize;      // Page size in chartctrl units.
    CSize m_PaperSize;            // Page size in device units.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARTCTRL_H__5DDBAE42_77C3_4344_A207_3F39B623343B__INCLUDED_)
