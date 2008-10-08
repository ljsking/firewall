/*
 *
 *	ChartLineSerie.cpp
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
 *	History:
 *		- 25/03/2008: Line series with a width > 1 can now have a style other than solid
 *					  (thanks to Bruno Lavier).
 *		- 12/08/2008: Performance fix: pen use the PS_GEOMETRIC style only when necessary
 *					  (thanks to Nick Holgate).
 *
 *
 */

#include "stdafx.h"
#include "ChartLineSerie.h"
#include "ChartCtrl.h"

#include "Math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartLineSerie::CChartLineSerie(CChartCtrl* pParent) : CChartSerie(pParent,stLineSerie)
{
	m_iLineWidth = 1;
	m_iPenStyle = PS_SOLID;
}

CChartLineSerie::~CChartLineSerie()
{

}

void CChartLineSerie::SetPenStyle(int NewStyle)  
{ 
	m_iPenStyle = NewStyle; 
	m_pParent->RefreshCtrl();
}

void CChartLineSerie::SetWidth(int NewValue)  
{ 
	m_iLineWidth = NewValue; 
	m_pParent->RefreshCtrl();
}

void CChartLineSerie::DrawAll(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	int iFirst=0, iLast=0;
	GetVisiblePoints(iFirst,iLast);

	if (iFirst>0)
		iFirst--;
	if (iLast<(int)GetPointsCount()-1)
		iLast++;
	if (pDC->GetSafeHdc())
	{
		CPen NewPen;
		CPen ShadowPen;
		if (m_iPenStyle != PS_SOLID)
		{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_ObjectColor;
			NewPen.CreatePen(PS_GEOMETRIC | m_iPenStyle, m_iLineWidth, &lb);
			lb.lbColor = m_ShadowColor;
			ShadowPen.CreatePen(PS_GEOMETRIC | m_iPenStyle, m_iLineWidth, &lb);
		}
		else
		{
			NewPen.CreatePen(m_iPenStyle, m_iLineWidth, m_ObjectColor);
			ShadowPen.CreatePen(m_iPenStyle, m_iLineWidth, m_ShadowColor);
		}
		CPen* pOldPen;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldPen = pDC->SelectObject(&NewPen);

		for (m_iLastDrawnPoint=iFirst;m_iLastDrawnPoint<iLast;m_iLastDrawnPoint++)
		{
			//We don't draw a line between the origin and the first point -> we must have
			// a least 2 points before begining drawing
			
			CPoint ScreenPoint;

			if (m_bShadow)
			{
				pDC->SelectObject(&ShadowPen);
				ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);
				pDC->MoveTo(ScreenPoint.x+m_iShadowDepth,ScreenPoint.y+m_iShadowDepth);
				ValueToScreen(m_vPoints[m_iLastDrawnPoint+1].X,m_vPoints[m_iLastDrawnPoint+1].Y,ScreenPoint);
				pDC->LineTo(ScreenPoint.x+m_iShadowDepth,ScreenPoint.y+m_iShadowDepth);
				
				pDC->SelectObject(&NewPen);
			}

			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);
			pDC->MoveTo(ScreenPoint.x,ScreenPoint.y);
			ValueToScreen(m_vPoints[m_iLastDrawnPoint+1].X,m_vPoints[m_iLastDrawnPoint+1].Y,ScreenPoint);
			pDC->LineTo(ScreenPoint.x,ScreenPoint.y);
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldPen);
		NewPen.DeleteObject();
		ShadowPen.DeleteObject();
	}
}

void CChartLineSerie::Draw(CDC* pDC)
{
	if (!m_bIsVisible)
		return;

	// If shadow is enabled, the all series must be redrawn.
	if (m_bShadow)
	{
		DrawAll(pDC);
		return;
	}

	if (pDC->GetSafeHdc())
	{
		CPen NewPen;
		if (m_iPenStyle != PS_SOLID)
		{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_ObjectColor;
			NewPen.CreatePen(PS_GEOMETRIC | m_iPenStyle, m_iLineWidth, &lb);
		}
		else
		{
			NewPen.CreatePen(m_iPenStyle, m_iLineWidth, m_ObjectColor);
		}
		CPen* pOldPen;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldPen = pDC->SelectObject(&NewPen);

		//Draw all points that haven't been drawn yet
		for (m_iLastDrawnPoint;m_iLastDrawnPoint<(int)GetPointsCount()-1;m_iLastDrawnPoint++)
		{
			//We don't draw a line between the origin and the first point -> we must have
			// a least 2 points before begining drawing
		//	if (m_vPoints<1)
		//		break;

			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);
			pDC->MoveTo(ScreenPoint.x,ScreenPoint.y);

			ValueToScreen(m_vPoints[m_iLastDrawnPoint+1].X,m_vPoints[m_iLastDrawnPoint+1].Y,ScreenPoint);
			pDC->LineTo(ScreenPoint.x,ScreenPoint.y);
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldPen);
		DeleteObject(NewPen);
	}
}

void CChartLineSerie::DrawLegend(CDC *pDC, const CRect& rectBitmap) const
{
	if (m_strSerieName== _T(""))
		return;

	//Draw line:
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = m_ObjectColor;
	CPen NewPen(PS_GEOMETRIC | m_iPenStyle,m_iLineWidth,&lb);
	CPen* pOldPen = pDC->SelectObject(&NewPen);
	pDC->MoveTo(rectBitmap.left,rectBitmap.CenterPoint().y);
	pDC->LineTo(rectBitmap.right,rectBitmap.CenterPoint().y);
	pDC->SelectObject(pOldPen);
	DeleteObject(NewPen);
}
