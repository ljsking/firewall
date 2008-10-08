/*
 *
 *	ChartSerie.h
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

#if !defined(AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_)
#define AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"
#include "ChartAxis.h"

#include <vector>
#include "ChartString.h"

class CChartLineSerie;
class CChartPointsSerie;
class CChartLegend;



class CChartSerie : public CChartObject  
{
	friend CChartCtrl;
	friend CChartLegend;

public:
	CChartPointsSerie* GetAsPoints();
	CChartLineSerie*   GetAsLine();

	void AddPoint(double X, double Y);
	void AddPoints(double *X, double *Y, int Count);
	void SetPoints(double *X, double *Y, int Count);
	void RemovePointsFromBegin(unsigned Count);
	void RemovePointsFromEnd(unsigned Count);
	void ClearSerie();

	size_t GetPointsCount() const  { return m_vPoints.size(); }
	double GetYPointValue(int PointIndex) const;
	double GetXPointValue(int PointIndex) const;
	void   SetYPointValue(int PointIndex, double NewVal);
	void   SetXPointValue(int PointIndex, double NewVal);	
	
	bool GetSerieYMinMax(double& Min, double& Max)  const;
	bool GetSerieXMinMax(double& Min, double& Max)  const;

	void		 SetName(const TChartString& NewName);
	TChartString GetName() const              { return m_strSerieName; }

	void ValueToScreen(double XValue, double YValue, CPoint& ScreenPoint)  const;
	double YScreenToValue(long YScreenCoord)  const;
	double XScreenToValue(long XScreenCoord)  const;

//	void RefreshAutoAxes();
	bool SetHorizontalAxis(bool bSecond);
	bool SetVerticalAxis(bool bSecond);

	CChartSerie(CChartCtrl* pParent,int Type);
	virtual ~CChartSerie();

	enum SeriesType
	{
		stLineSerie=0,
		stPointsSerie=1,
		stSurfaceSerie=2,
		stBarSerie=3
	};
	enum SeriesOrdering
	{
		soNoOrdering,
		soXOrdering,
		soYOrdering
	};

	virtual void SetSeriesOrdering(SeriesOrdering newOrdering);
	SeriesOrdering GetSeriesOrdering() const		{ return m_Ordering; } 

protected:
    struct SChartPoint
    {
    	double X;
    	double Y;
    };
	std::vector<SChartPoint>   m_vPoints; 
	
	int m_iLastDrawnPoint;			// Index of the last point drawn

	CChartAxis* m_pVerticalAxis;    // Related vertical axis
	CChartAxis* m_pHorizontalAxis;  // Related horizontal axis

	TChartString m_strSerieName;	// Name displayed in the legend
	int m_iSerieType;			    // Type of the serie (ligne, point, surface, ...)

	double m_dXMinVal;
	double m_dXMaxVal;
	double m_dYMinVal;
	double m_dYMaxVal;

	SeriesOrdering m_Ordering;

	void GetVisiblePoints(int& iFirst, int& iLast);

private:
    virtual void DrawLegend(CDC* pDC, const CRect& rectBitmap) const =0;

	virtual void Draw(CDC* pDC) =0;
	virtual void DrawAll(CDC *pDC) =0;

	void RefreshMinMax();
	void InsertNewPoint(const SChartPoint& NewPoint);

	struct SPointSort
	{
		bool operator()(SChartPoint& ptStart, SChartPoint& ptEnd)
		{
			if (m_bXSort)
				return ptStart.X < ptEnd.X;
			else
				return ptStart.Y < ptEnd.Y;
		}

		bool m_bXSort;
	};
	void ReorderPoints();
};

#endif // !defined(AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_)
