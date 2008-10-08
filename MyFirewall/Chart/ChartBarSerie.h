/*
 *
 *	ChartBarSerie.h
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
#include "ChartSerie.h"
#include "ChartGradient.h"
#include <list>

class CChartBarSerie : public CChartSerie
{
public:
	CChartBarSerie(CChartCtrl* pParent);
	~CChartBarSerie();

	void SetHorizontal(bool bHorizontal);
	bool GetHorizontal() const		  { return m_bHorizontal; }

	void SetBorderColor(COLORREF BorderColor);
	COLORREF GetBorderColor() const			   { return m_BorderColor; }
	void SetBorderWidth(int Width);
	int  GetBorderWidth() const		{ return m_iBorderWidth;  }
	void SetBarWidth(int Width);
	int  GetBarWidth() const		{ return m_iBarWidth;  }

	void SetGroupId(unsigned GroupId);
	unsigned GetGroupId() const			{ return m_uGroupId;    }

	void ShowGradient(bool bShow);
	void SetGradient(COLORREF GradientColor, EGradientType GradientType);

	static void SetInterSpace(int Space)	{ m_iInterSpace = Space; }
	static int  GetInterSpace()				{ return m_iInterSpace;  }

	void SetBaseLine(bool bAutomatic, double dBaseLine)
	{
		m_bAutoBaseLine = bAutomatic;
		m_dBaseLine = dBaseLine;
	}

private:
	void DrawLegend(CDC* pDC, const CRect& rectBitmap) const;

	void Draw(CDC* pDC);
	void DrawAll(CDC *pDC);
	void DrawBar(CDC* pDC, CBrush* pFillBrush, CBrush* pBorderBrush, 
				 int XPos, int YPos);

	int GetSerieStartPos();

	typedef std::list<CChartBarSerie*> TBarSeriesList;
	static int m_iInterSpace;	// Space between two series of the same group.
	static TBarSeriesList m_lstBarSeries;
	
	bool m_bHorizontal;

	// The base line specifies the position (in terms of the value
	// axis) where the bars start from. If m_bAutoBaseLine is true
	// they start from the axis position.
	double m_dBaseLine;
	bool m_bAutoBaseLine;

	// Specifies to which group this series belongs to. Series in the same group are
	// 'stacked' next to each other.
	unsigned m_uGroupId;		

	int m_iBarWidth;
	int m_iBorderWidth;
	COLORREF m_BorderColor;

	bool m_bGradient;
	COLORREF m_GradientColor;
	EGradientType m_GradientType;
};
