// TesterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FilterHelper.h"
#include "..\\myDriver\\Filter.h"
#include "Tester.h"
#include "TesterDlg.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString GetLocalIP()        // local IP 획득
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	CString ip; // 여기에 lcoal ip가 저장됩니다.
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}      
		WSACleanup( );
	} 
	return ip;
}

// CTesterDlg 대화 상자

CTesterDlg::CTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTesterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTesterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTesterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTesterDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTesterDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTesterDlg 메시지 처리기

BOOL CTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//we load the IPFilter Driver
	filterDriver.LoadDriver(_T("IpFilterDriver"), _T("System32\\Drivers\\IpFltDrv.sys"), NULL, TRUE);
	//we don't deregister the driver at destructor
	filterDriver.SetRemovable(FALSE);
	DWORD ret = helper.LoadDriver(_T("MyDriver"), NULL, NULL, TRUE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTesterDlg::OnBnClickedButton1()
{
	helper.WriteIo(START_IP_HOOK, NULL, 0);
}

void CTesterDlg::OnBnClickedButton2()
{
	helper.WriteIo(STOP_IP_HOOK, NULL, 0);
}

void CTesterDlg::OnBnClickedButton3()
{
	UpdateData();

	DWORD result;

	IPFilter pf;

	char ascii[256];
	wcstombs( ascii, GetLocalIP().GetBuffer(), 256 );

  	pf.protocol = 0;									
	pf.destinationIp = inet_addr("222.122.84.250");
	pf.sourceIp = inet_addr(ascii);
	pf.destinationMask = inet_addr("255.255.255.255");
	pf.sourceMask = inet_addr("255.255.255.255");
	pf.destinationPort = htons(0);
	pf.sourcePort = htons(0);
	pf.drop = TRUE;

	result = AddFilter(pf);		//send the rule
}

BOOL CTesterDlg::AddFilter(IPFilter &pf)
{
	//we send the rule to the driver
	DWORD result = helper.WriteIo(ADD_FILTER, &pf, sizeof(pf));

	/*if (result != DRV_SUCCESS) 
		return FALSE;
	else*/
		return TRUE;
}