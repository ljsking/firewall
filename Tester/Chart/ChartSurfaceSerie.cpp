/*
 *
 *	ChartSurfaceSerie.cpp
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
#include "ChartSurfaceSerie.h"
#include "ChartCtrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartSurfaceSerie::CChartSurfaceSerie(CChartCtrl* pParent) 
 : CChartSerie(pParent,stSurfaceSerie), m_FillStyle(fsHatchDownDiag), m_bHorizontal(true)
{

}

CChartSurfaceSerie::~CChartSurfaceSerie()
{

}

void CChartSurfaceSerie::Draw(CDC* pDC)
{
	DrawAll(pDC);
}

void CChartSurfaceSerie::DrawAll(CDC* pDC)
{
	int iFirst=0, iLast=0;
	GetVisiblePoints(iFirst,iLast);

	if (iFirst>0)
		iFirst--;
	if (iLast<(int)GetPointsCount())
		iLast++;
	int iCount = iLast - iFirst;
	CPoint* pPoints = new CPoint[iCount+2];

	CBrush NewBrush;
	if (m_FillStyle == fsSolid)
		NewBrush.CreateSolidBrush(m_ObjectColor);
	else
	{
		int nIndex = 0;
		switch (m_FillStyle)
		{
		case fsHatchDownDiag:
			nIndex = HS_FDIAGONAL;
			break;
		case fsHatchUpDiag:
			nIndex = HS_BDIAGONAL;
			break;
		case fsHatchCross:
			nIndex = HS_CROSS;
			break;
		case fsHatchDiagCross:
			nIndex = HS_DIAGCROSS;
			break;
		case fsHatchHorizontal:
			nIndex = HS_HORIZONTAL;
			break;
		case fsHatchVertical:
			nIndex = HS_VERTICAL;
			break;
		}
		NewBrush.CreateHatchBrush(nIndex,m_ObjectColor);
	}

	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	for (int i=iFirst; i<iLast; i++)
	{
		ValueToScreen(m_vPoints[i].X,m_vPoints[i].Y,pPoints[i-iFirst+1]);
	}

	if (m_bHorizontal)
	{
		pPoints[0].x = pPoints[1].x;
		pPoints[iCount+1].x = pPoints[iCount].x;

		double Position = m_pHorizontalAxis->GetPosition()/100.00;
		int AxisPos = m_ObjectRect.top + (int)(Position * (m_ObjectRect.bottom-m_ObjectRect.top));

		pPoints[0].y = AxisPos;
		pPoints[iCount+1].y = AxisPos;
	}
	else
	{
		pPoints[0].y = pPoints[1].y;
		pPoints[iCount+1].y = pPoints[iCount].y;

		double Position = m_pVerticalAxis->GetPosition()/100.00;
		int AxisPos = m_ObjectRect.left + (int)(Position * (m_ObjectRect.right-m_ObjectRect.left));

		pPoints[0].x = AxisPos;
		pPoints[iCount+1].x = AxisPos;
	}

	pDC->SetBkMode(TRANSPARENT);
	//To have lines limited in the drawing rectangle :
	CRect TempClipRect(m_ObjectRect);
	TempClipRect.DeflateRect(1,1);
	pDC->SetBkMode(TRANSPARENT);
	pDC->IntersectClipRect(TempClipRect);

	pDC->Polygon(pPoints,iCount+2);
	pDC->SelectClipRgn(NULL);
	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);

	delete[] pPoints;
}

void CChartSurfaceSerie::DrawLegend(CDC* pDC, const CRect& rectBitmap) const
{
	if (m_strSerieName== _T(""))
		return;

	// Draw the bitmap
	CBrush NewBrush;
	if (m_FillStyle == fsSolid)
		NewBrush.CreateSolidBrush(m_ObjectColor);
	else
	{
		int nIndex = 0;
		switch (m_FillStyle)
		{
		case fsHatchDownDiag:
			nIndex = HS_FDIAGONAL;
			break;
		case fsHatchUpDiag:
			nIndex = HS_BDIAGONAL;
			break;
		case fsHatchCross:
			nIndex = HS_CROSS;
			break;
		case fsHatchDiagCross:
			nIndex = HS_DIAGCROSS;
			break;
		case fsHatchHorizontal:
			nIndex = HS_HORIZONTAL;
			break;
		case fsHatchVertical:
			nIndex = HS_VERTICAL;
			break;
		}
		NewBrush.CreateHatchBrush(nIndex,m_ObjectColor);
	}

	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	pDC->Rectangle(rectBitmap);

	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);
}

void CChartSurfaceSerie::SetFillStyle(FillStyle NewStyle)  
{ 
	m_FillStyle = NewStyle; 
	m_pParent->RefreshCtrl();
}

void CChartSurfaceSerie::SetHorizontal(bool bHoriz) 
{ 
	m_bHorizontal = bHoriz; 
	if (m_bHorizontal)
		SetSeriesOrdering(soXOrdering);
	else
		SetSeriesOrdering(soYOrdering);
	m_pParent->RefreshCtrl();
}

void CChartSurfaceSerie::SetSeriesOrdering(SeriesOrdering newOrdering)
{
	TRACE("Can't change the series ordering of a surface series.");
}
