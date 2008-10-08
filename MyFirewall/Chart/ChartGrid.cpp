/*
 *
 *	ChartGrid.cpp
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
#include "ChartGrid.h"
#include "ChartAxis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartGrid::CChartGrid(CChartCtrl* pParent, bool bHoriz) 
  : CChartObject(pParent), m_bIsHorizontal(bHoriz), m_vecTickPos()
{
	m_ObjectColor = RGB(128,128,128);
}

CChartGrid::~CChartGrid()
{

}

void CChartGrid::AddTick(int Position)
{
	m_vecTickPos.push_back(Position);
}

void CChartGrid::ClearTicks()
{
	m_vecTickPos.clear();
}

void CChartGrid::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (!pDC->GetSafeHdc() )
		return;

	CPen* pOldPen;
	CPen NewPen(PS_SOLID,0,m_ObjectColor);
	pOldPen = pDC->SelectObject(&NewPen);

	list<int>::iterator iter = m_vecTickPos.begin();
	int ActuPosition = 0;
	for (iter; iter!=m_vecTickPos.end(); iter++)
	{
		ActuPosition = *iter;
		if (!m_bIsHorizontal)
		{
			int ActuX = m_ObjectRect.left;

			while (true)
			{
				if (!Clip(ActuX,ActuPosition))
					break;
				pDC->MoveTo(ActuX,ActuPosition);

				ActuX += 3;
				Clip(ActuX,ActuPosition);
				pDC->LineTo(ActuX,ActuPosition);

				ActuX += 3;
			}
		}
		else
		{
			int ActuY = m_ObjectRect.bottom;

			while (true)
			{
				if (!Clip(ActuPosition,ActuY))
					break;
				pDC->MoveTo(ActuPosition,ActuY);

				ActuY -= 3;
				Clip(ActuPosition,ActuY);
				pDC->LineTo(ActuPosition,ActuY);

				ActuY -= 3;
			}
		}
	}

	pDC->SelectObject(pOldPen);
	DeleteObject(NewPen);
}
