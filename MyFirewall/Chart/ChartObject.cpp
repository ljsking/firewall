/*
 *
 *	ChartObject.cpp
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
#include "ChartCtrl.h"

CChartObject::CChartObject(CChartCtrl* pOwner)
{
	m_ObjectRect.left = m_ObjectRect.right = 0;
	m_ObjectRect.top = m_ObjectRect.bottom = 0;
	m_ObjectColor = RGB(0,0,0);
	m_pParent = pOwner;
	m_bIsVisible = true;
	m_bShadow = false;
	m_ShadowColor = RGB(150,150,150);
	m_iShadowDepth = 2;
}

CChartObject::~CChartObject()
{
}

void CChartObject::SetVisible(bool bVisible)
{ 
	m_bIsVisible = bVisible; 
	m_pParent->RefreshCtrl();
}

void CChartObject::SetColor(COLORREF NewColor)	   
{ 
	m_ObjectColor = NewColor; 
	m_pParent->RefreshCtrl();
}

void CChartObject::SetShadowColor(COLORREF NewColor) 
{ 
	m_ShadowColor = NewColor; 
	m_pParent->RefreshCtrl();
}

void CChartObject::EnableShadow(bool bEnable)
{
	m_bShadow = bEnable;
	m_pParent->RefreshCtrl();
}

void CChartObject::SetShadowDepth(int Depth)
{ 
	m_iShadowDepth = Depth; 
	m_pParent->RefreshCtrl();
}
