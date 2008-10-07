/*
 *
 *	ChartLegend.h
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

#if !defined(AFX_CHARTLEGEND_H__CD72E5A0_8F52_472A_A611_C588F642080B__INCLUDED_)
#define AFX_CHARTLEGEND_H__CD72E5A0_8F52_472A_A611_C588F642080B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"
#include "ChartCtrl.h"

#include "ChartString.h"

class CChartSerie;

class CChartLegend : public CChartObject  
{
	friend CChartCtrl;

public:
	void SetFont(int iPointSize, const TChartString& strFaceName);

	CChartLegend(CChartCtrl* pParent);
	virtual ~CChartLegend();

	enum DockSide
	{
		dsDockRight,
		dsDockLeft,
		dsDockTop,
		dsDockBottom
	};

	void DockLegend(DockSide dsSide);
	void UndockLegend(int iLeftPos, int iTopPos);

	void SetTransparent(bool bTransparent);  
	void SetHorizontalMode(bool bHorizontal);

private:
	void Draw(CDC* pDC);
	void ClipArea(CRect& rcControl, CDC* pDC);
	void UpdatePosition(CDC* pDC, const CRect& rcControl);

	TChartString m_strFontName;
	int          m_iFontSize;

	bool m_bDocked;	// true if the legend is docked
	DockSide m_DockSide;

	// If the legend is not docked:
	int m_iLeftPos;
	int m_iTopPos;

	bool m_bIsTransparent;
	bool m_bIsHorizontal;

	CSize m_BitmapSize;
};

#endif // !defined(AFX_CHARTLEGEND_H__CD72E5A0_8F52_472A_A611_C588F642080B__INCLUDED_)
