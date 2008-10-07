/*
 *
 *	ChartPointsSerie.cpp
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
 *		- 07/07/2008: Last point of the series was not displayed. Fixed.
 */

#include "stdafx.h"
#include "ChartPointsSerie.h"
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

CChartPointsSerie::CChartPointsSerie(CChartCtrl* pParent) 
 : CChartSerie(pParent,stPointsSerie), m_iPointType(ptEllipse), m_iXPointSize(5),
   m_iYPointSize(5)
{
}

CChartPointsSerie::~CChartPointsSerie()
{

}
void CChartPointsSerie::SetPointSize(int XSize, int YSize)
{
	m_iXPointSize = XSize;
	m_iYPointSize = YSize;
	m_pParent->RefreshCtrl();
}

void CChartPointsSerie::SetPointType(PointType Type)  
{ 
	m_iPointType = Type; 
	m_pParent->RefreshCtrl();
}

void CChartPointsSerie::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc())
	{
		CBrush NewBrush(m_ObjectColor);
		CBrush ShadowBrush(m_ShadowColor);
		CPen ShadowPen(PS_SOLID,1,m_ShadowColor);
		CPen* pOldPen;
		CBrush* pOldBrush;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldBrush = pDC->SelectObject(&NewBrush);

		//Draw all points that haven't been drawn yet
		for (m_iLastDrawnPoint;m_iLastDrawnPoint<(int)GetPointsCount();m_iLastDrawnPoint++)
		{
			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);

			CRect PointRect;
			PointRect.SetRect(ScreenPoint.x-m_iXPointSize/2,ScreenPoint.y-m_iYPointSize/2,ScreenPoint.x+m_iXPointSize/2,ScreenPoint.y+m_iYPointSize/2);
			CRect ShadowRect = PointRect + CSize(m_iShadowDepth,m_iShadowDepth);

			switch(m_iPointType)
			{
			case ptEllipse:
				if (m_bShadow)
				{
					pOldPen = pDC->SelectObject(&ShadowPen);
					pDC->SelectObject(&ShadowBrush);
					pDC->Ellipse(ShadowRect);
					pDC->SelectObject(&NewBrush);
					pDC->SelectObject(pOldPen);
				}
				pDC->Ellipse(PointRect);
				break;

			case ptRectangle:
				if (m_bShadow)
				{
					pOldPen = pDC->SelectObject(&ShadowPen);
					pDC->SelectObject(&ShadowBrush);
					pDC->Rectangle(ShadowRect);
					pDC->SelectObject(&NewBrush);
					pDC->SelectObject(pOldPen);
				}
				pDC->Rectangle(PointRect);
				break;

			case ptTriangle:
				{
					CPoint TrPoints[3];
					TrPoints[0].x = PointRect.left;
					TrPoints[0].y = PointRect.bottom;
					TrPoints[1].x = PointRect.right;
					TrPoints[1].y = PointRect.bottom;
					TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
					TrPoints[2].y = PointRect.top;

					if (m_bShadow)
					{
						CPoint ShadowPoints[3];
						for (int i=0;i<3;i++)
						{
							ShadowPoints[i] = TrPoints[i] + CSize(m_iShadowDepth,m_iShadowDepth);
						}

						pOldPen = pDC->SelectObject(&ShadowPen);
						pDC->SelectObject(&ShadowBrush);
						pDC->Polygon(ShadowPoints,3);
						pDC->SelectObject(&NewBrush);
						pDC->SelectObject(pOldPen);
					}
					pDC->Polygon(TrPoints,3);
				}
				break;
			}

		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldBrush);
		DeleteObject(NewBrush);
		DeleteObject(ShadowBrush);
		DeleteObject(ShadowPen);
	}
}

void CChartPointsSerie::DrawAll(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	CBrush NewBrush(m_ObjectColor);
	CBrush ShadowBrush(m_ShadowColor);
	CPen ShadowPen(PS_SOLID,1,m_ShadowColor);
	CPen* pOldPen;
	CBrush* pOldBrush;

	int iFirst=0, iLast=0;
	GetVisiblePoints(iFirst,iLast);

	if (pDC->GetSafeHdc())
	{
		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldBrush = pDC->SelectObject(&NewBrush);

		for (m_iLastDrawnPoint=iFirst;m_iLastDrawnPoint<=iLast;m_iLastDrawnPoint++)
		{
			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);

			CRect PointRect;
			PointRect.SetRect(ScreenPoint.x-m_iXPointSize/2,ScreenPoint.y-m_iYPointSize/2,ScreenPoint.x+m_iXPointSize/2,ScreenPoint.y+m_iYPointSize/2);
			CRect ShadowRect = PointRect + CSize(m_iShadowDepth,m_iShadowDepth);

			switch(m_iPointType)
			{
			case ptEllipse:
				if (m_bShadow)
				{
					pOldPen = pDC->SelectObject(&ShadowPen);
					pDC->SelectObject(&ShadowBrush);
					pDC->Ellipse(ShadowRect);
					pDC->SelectObject(&NewBrush);
					pDC->SelectObject(pOldPen);
				}
				pDC->Ellipse(PointRect);
				break;

			case ptRectangle:
				if (m_bShadow)
				{
					pOldPen = pDC->SelectObject(&ShadowPen);
					pDC->SelectObject(&ShadowBrush);
					pDC->Rectangle(ShadowRect);
					pDC->SelectObject(&NewBrush);
					pDC->SelectObject(pOldPen);
				}
				pDC->Rectangle(PointRect);
				break;

			case ptTriangle:
				{
					CPoint TrPoints[3];
					TrPoints[0].x = PointRect.left;
					TrPoints[0].y = PointRect.bottom;
					TrPoints[1].x = PointRect.right;
					TrPoints[1].y = PointRect.bottom;
					TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
					TrPoints[2].y = PointRect.top;

					if (m_bShadow)
					{
						CPoint ShadowPoints[3];
						for (int i=0;i<3;i++)
						{
							ShadowPoints[i] = TrPoints[i] + CSize(m_iShadowDepth,m_iShadowDepth);
						}

						pOldPen = pDC->SelectObject(&ShadowPen);
						pDC->SelectObject(&ShadowBrush);
						pDC->Polygon(ShadowPoints,3);
						pDC->SelectObject(&NewBrush);
						pDC->SelectObject(pOldPen);
					}
					pDC->Polygon(TrPoints,3);
				}
				break;
			}
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldBrush);
		DeleteObject(NewBrush);
	}
}

void CChartPointsSerie::DrawLegend(CDC *pDC, const CRect& rectBitmap) const
{
	if (m_strSerieName== _T(""))
		return;

	CRect PointRect(0,0,m_iXPointSize,m_iYPointSize);
	if ( (rectBitmap.Height()>m_iYPointSize) && (rectBitmap.Width()>m_iXPointSize) )
	{
		int XOffset = rectBitmap.left + rectBitmap.Width()/2 - m_iXPointSize/2;
		int YOffset = rectBitmap.top + rectBitmap.Height()/2 - m_iYPointSize/2;
		PointRect.OffsetRect(XOffset,YOffset);
	}
	else
		PointRect = rectBitmap;

	CBrush NewBrush(m_ObjectColor);
	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	switch(m_iPointType)
	{
	case ptEllipse:
		pDC->Ellipse(PointRect);
		break;

	case ptRectangle:
		pDC->Rectangle(PointRect);
		break;

	case ptTriangle:
		{
			CPoint TrPoints[3];
			TrPoints[0].x = PointRect.left;
			TrPoints[0].y = PointRect.bottom;
			TrPoints[1].x = PointRect.right;
			TrPoints[1].y = PointRect.bottom;
			TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
			TrPoints[2].y = PointRect.top;

			pDC->Polygon(TrPoints,3);
		}
		break;
	}

	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);
}
