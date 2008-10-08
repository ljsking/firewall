/*
 *
 *	ChartLegend.cpp
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
 *		- 02/03/2008: Legend can now be docked on any side or can be floating.
 *		- 02/03/2008: Added support for transparent legend.
 *		- 24/03/2008: Bug fix for invisible series.
 *		- 28/03/2008: Support for horizontal legend.
 *		- 28/03/2008: Bitmap size is now the same for all series.
 *
 */

#include "stdafx.h"
#include "ChartLegend.h"
#include "ChartSerie.h"
#include "ChartCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartLegend::CChartLegend(CChartCtrl* pParent):CChartObject(pParent)
{
	m_ObjectColor = RGB(255,255,255);
	m_iFontSize = 100;
	m_strFontName = _T("Times New Roman");

	m_bIsVisible = false;

	m_bDocked = true;
	m_DockSide = dsDockRight;
	m_iLeftPos = m_iTopPos = 0;
	m_bIsTransparent = false;
	m_bIsHorizontal = false;

	m_bShadow = true;
	m_iShadowDepth = 3;
	m_BitmapSize.cx = 16;
	m_BitmapSize.cy = 16;
}

CChartLegend::~CChartLegend()
{
}

void CChartLegend::SetFont(int iPointSize, const TChartString& strFaceName)
{
	m_iFontSize = iPointSize;
	m_strFontName = strFaceName;
	m_pParent->RefreshCtrl();
}

void CChartLegend::SetTransparent(bool bTransparent)
{ 
	m_bIsTransparent = bTransparent; 
	m_pParent->RefreshCtrl();
}

void CChartLegend::SetHorizontalMode(bool bHorizontal)
{
	m_bIsHorizontal = bHorizontal;
	m_pParent->RefreshCtrl();
}

void CChartLegend::DockLegend(DockSide dsSide)
{
	m_bDocked = true;
	m_DockSide = dsSide;
	m_pParent->RefreshCtrl();
}

void CChartLegend::UndockLegend(int iLeftPos, int iTopPos)
{
	m_bDocked = false;
	m_iLeftPos = iLeftPos;
	m_iTopPos = iTopPos;
	m_pParent->RefreshCtrl();
}

void CChartLegend::ClipArea(CRect& rcControl, CDC* pDC)
{
	UpdatePosition(pDC,rcControl);
	if (m_ObjectRect.IsRectEmpty())
		return;

	if (m_bDocked)
	{
		switch (m_DockSide)
		{
		case dsDockRight:
			rcControl.right = m_ObjectRect.left + 2;
			break;
		case dsDockLeft:
			rcControl.left = m_ObjectRect.right - 2;
			break;
		case dsDockTop:
			rcControl.top = m_ObjectRect.bottom + 2;
			break;
		case dsDockBottom:
			rcControl.bottom = m_ObjectRect.top - 2;
			break;
		}
	}
}

void CChartLegend::UpdatePosition(CDC* pDC, const CRect& rcControl)
{
	CRect NewPosition;
	NewPosition.SetRectEmpty();
	if (!m_bIsVisible)
	{
		SetRect(NewPosition);
		return;
	}

	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize,m_strFontName.c_str(),pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	int Height = 0;		
	int Width = 0;		
	int MaxText = 0;
	CSize TextSize;

	size_t SeriesCount = m_pParent->GetSeriesCount();
	int Drawn = 0;
	for (size_t i=0;i<SeriesCount;i++)
	{
		CChartSerie* pSerie = m_pParent->GetSerie(i);
		if ( (pSerie->GetName() == _T("")) || !pSerie->IsVisible() )
			continue;

		Drawn++;
		TextSize = pDC->GetTextExtent(pSerie->GetName().c_str());

		if (!m_bIsHorizontal)
		{
			if (TextSize.cy>m_BitmapSize.cy)
				Height += TextSize.cy + 2;
			else
				Height += m_BitmapSize.cy + 2;

			if (TextSize.cx > MaxText)
				MaxText = TextSize.cx;
		}
		else
		{
			Width += TextSize.cx + 4 + m_BitmapSize.cx + 10;
			if (TextSize.cy > MaxText)
				MaxText = TextSize.cy;
		}
	}
	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);

	if (!Drawn)
	{
		SetRect(NewPosition);
		return;
	}

	if (!m_bIsHorizontal)
	{
		Width += MaxText + m_BitmapSize.cx + 12;
		Height += 4 + 4 - 2;	// Top and bottom margins. -2 because space counted once too much
	}
	else
	{
		Width += 2 + 2 - 10;
		Height = 4 + max(m_BitmapSize.cy,MaxText) + 4;
	}
	
	if (!m_bDocked)
	{
		NewPosition.top = m_iTopPos;
		NewPosition.left = m_iLeftPos;
		NewPosition.bottom = m_iTopPos + Height + 2;
		NewPosition.right = m_iLeftPos + Width;
	}
	else
	{
		switch (m_DockSide)
		{
		case dsDockRight:
			NewPosition.top = ((rcControl.bottom-rcControl.top)/2) - ((Height + 2)/2);
			NewPosition.left = rcControl.right - (Width + 6);
			NewPosition.bottom = NewPosition.top + Height;
			NewPosition.right = NewPosition.left + Width;
			break;
		case dsDockLeft:
			NewPosition.top = ((rcControl.bottom-rcControl.top)/2) - ((Height + 2)/2);
			NewPosition.left = rcControl.left + 3; 
			NewPosition.bottom = NewPosition.top + Height;
			NewPosition.right = NewPosition.left + Width;
			break;
		case dsDockTop:
			NewPosition.top = rcControl.top + 3;  //((rcControl.bottom-rcControl.top)/2) - ((Height + 2)/2);
			NewPosition.left = ((rcControl.right-rcControl.left)/2) - (Width/2);  // rcControl.left + 3; 
			NewPosition.bottom = NewPosition.top + Height;
			NewPosition.right = NewPosition.left + Width;
			break;
		case dsDockBottom:
			NewPosition.top = rcControl.bottom - (Height + 2);  //((rcControl.bottom-rcControl.top)/2) - ((Height + 2)/2);
			NewPosition.left = ((rcControl.right-rcControl.left)/2) - (Width/2);  // rcControl.left + 3; 
			NewPosition.bottom = NewPosition.top + Height;
			NewPosition.right = NewPosition.left + Width;
			break;
		}
	}
	SetRect(NewPosition);
}

void CChartLegend::Draw(CDC *pDC)
{
	if (!pDC->GetSafeHdc())
		return;
	if (!m_bIsVisible)
		return;
	if (m_ObjectRect.IsRectEmpty())
		return;

	CPen SolidPen(PS_SOLID,0,RGB(0,0,0));
	CPen* pOldPen;
	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize,m_strFontName.c_str(),pDC);

	// Draw the shadow
	if (m_bShadow)
	{
		CRect ShadowRect = m_ObjectRect;
		ShadowRect.OffsetRect(m_iShadowDepth,m_iShadowDepth);
		CBrush BrushShadow;
		BrushShadow.CreateSolidBrush(m_ShadowColor) ;
		pDC->FillRect(ShadowRect,&BrushShadow);
	}

	if (!m_bIsTransparent)
	{
		//Fill back color
		CBrush BrushBack;
		BrushBack.CreateSolidBrush(m_ObjectColor) ;
		pDC->FillRect(m_ObjectRect,&BrushBack);
	}

	pOldFont = pDC->SelectObject(&NewFont);
	pOldPen = pDC->SelectObject(&SolidPen);

	//Draw rectangle:
	pDC->MoveTo(m_ObjectRect.left,m_ObjectRect.top);
	pDC->LineTo(m_ObjectRect.right,m_ObjectRect.top);
	pDC->LineTo(m_ObjectRect.right,m_ObjectRect.bottom);
	pDC->LineTo(m_ObjectRect.left,m_ObjectRect.bottom);
	pDC->LineTo(m_ObjectRect.left,m_ObjectRect.top);

	int iPrevMode = pDC->SetBkMode(TRANSPARENT);
	CRect rectBitmap(m_ObjectRect.left+2,m_ObjectRect.top+5,
					 m_ObjectRect.left+2+m_BitmapSize.cx,
					 m_ObjectRect.top+6+m_BitmapSize.cy);
	int SeriesCount = m_pParent->GetSeriesCount();
	for (int i=0;i<SeriesCount;i++)
	{
		CChartSerie* pSerie = m_pParent->GetSerie(i);
		if ( (pSerie->GetName() == _T("")) || !pSerie->IsVisible() )
			continue;

		int MaxHeight = 0;
		CSize TextSize = pDC->GetTextExtent(pSerie->GetName().c_str());
		if (TextSize.cy > m_BitmapSize.cy)
		{
			pDC->ExtTextOut(rectBitmap.right+4,rectBitmap.top,ETO_CLIPPED,NULL,pSerie->GetName().c_str(),NULL);
			CRect rectTemp(rectBitmap);
			int YOffset = TextSize.cy/2 - rectBitmap.Height()/2;
			rectTemp.OffsetRect(0,YOffset);
			pSerie->DrawLegend(pDC,rectTemp);
			MaxHeight = TextSize.cy;
		}
		else
		{
			int YOffset = rectBitmap.CenterPoint().y - TextSize.cy/2;
			pDC->ExtTextOut(rectBitmap.right+4,YOffset,ETO_CLIPPED,NULL,pSerie->GetName().c_str(),NULL);
			MaxHeight = m_BitmapSize.cy;
			pSerie->DrawLegend(pDC,rectBitmap);
		}
		

		if (!m_bIsHorizontal)
			rectBitmap.OffsetRect(0,MaxHeight+2);
		else
			rectBitmap.OffsetRect(m_BitmapSize.cx+4+TextSize.cx+10,0);
	}

	pDC->SetBkMode(iPrevMode);
	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SelectObject(pOldPen);
	DeleteObject(SolidPen);
}



