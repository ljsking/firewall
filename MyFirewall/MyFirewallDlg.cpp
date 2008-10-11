// TesterDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "DriverHelper.h"
#include "..\\myDriver\\Filter.h"
#include "MyFirewall.h"
#include "Port.h"
#include "Chart\\ChartCtrl.h"
#include "usagetrace.h"
#include "PortsManager.h"
#include "MyFirewallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMyFirewallDlg 대화 상자

CMyFirewallDlg::CMyFirewallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyFirewallDlg::IDD, pParent)
	, m_myIP(_T("192.168.0.1"))
	, m_sourceIP(_T("0"))
	, m_sourceMask(_T("255.255.255.255"))
	, m_sourcePort(_T("0"))
	, m_destIP(_T("0"))
	, m_destMask(_T("255.255.255.255"))
	, m_destPort(_T("0"))
	, m_word(_T(""))
	, m_protocolType(0)
	, update_interval(1000)
	, m_IPFilter(FALSE)
	, m_wordFilter(FALSE)
	, m_sessionFilter(FALSE)
	, m_portMonitor(FALSE)
	, m_maxSession(0)
	, m_nowSession(0)
	, m_total(0)
	, m_exceed(false)
	, m_idRule(0)
	, m_idWord(0)
	, m_started(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyFirewallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MY_IP, m_myIP);
	DDX_Text(pDX, IDC_ERULE_SOURCE_IP, m_sourceIP);
	DDX_Text(pDX, IDC_ERULE_SOURCE_MASK, m_sourceMask);
	DDX_Text(pDX, IDC_ERULE_SOURCE_PORT, m_sourcePort);
	DDX_Text(pDX, IDC_ERULE_DEST_IP, m_destIP);
	DDX_Text(pDX, IDC_ERULE_DEST_MASK, m_destMask);
	DDX_Text(pDX, IDC_ERULE_DEST_PORT, m_destPort);
	DDX_Control(pDX, IDC_LIST_RULE, m_listRules);
	DDX_Control(pDX, IDC_LIST_WORD, m_listWords);
	DDX_Text(pDX, IDC_EWORD, m_word);
	DDX_CBIndex(pDX, IDC_COMBO1, m_protocolType);
	DDX_Check(pDX, IDC_CHECK_RULE, m_IPFilter);
	DDX_Check(pDX, IDC_CHECK_WORD, m_wordFilter);
	DDX_Check(pDX, IDC_CHECK_SESSION, m_sessionFilter);
	DDX_Check(pDX, IDC_CHECK_MONITOR, m_portMonitor);
	DDX_Control(pDX, IDC_LIST_PORT, m_listPorts);
	DDX_Control(pDX, IDC_CHART, m_chartCtrl);
	DDX_Text(pDX, IDC_EMAXSESSION, m_maxSession);
	DDX_Text(pDX, IDC_ENOWSESSION, m_nowSession);
	DDX_Text(pDX, IDC_ETOTAL_USAGE, m_total);
}

BEGIN_MESSAGE_MAP(CMyFirewallDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BSTART, &CMyFirewallDlg::OnBnClickedBStart)
	ON_BN_CLICKED(IDC_BSTOP, &CMyFirewallDlg::OnBnClickedBStop)
	ON_BN_CLICKED(IDC_BRULE_ADD, &CMyFirewallDlg::OnBnClickedBRuleAdd)
	ON_BN_CLICKED(IDC_BWORD_ADD, &CMyFirewallDlg::OnBnClickedBwordAdd)
	ON_BN_CLICKED(IDC_BRULE_DELETE, &CMyFirewallDlg::OnBnClickedBruleDelete)
	ON_BN_CLICKED(IDC_BWORD_DELETE, &CMyFirewallDlg::OnBnClickedBwordDelete)
	ON_BN_CLICKED(IDC_CHECK_RULE, &CMyFirewallDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_WORD, &CMyFirewallDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_SESSION, &CMyFirewallDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_MONITOR, &CMyFirewallDlg::OnBnClickedCheck)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RULE, &CMyFirewallDlg::OnLvnItemchangedListRule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WORD, &CMyFirewallDlg::OnLvnItemchangedListWord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PORT, &CMyFirewallDlg::OnLvnItemchangedListPort)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyFirewallDlg 메시지 처리기

BOOL CMyFirewallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	GetLocalIP();
	char ascii[256];
	wcstombs( ascii, m_myIP, 256 );
	m_IP = inet_addr(ascii);

	//we load the IPFilter Driver
	filterDriver.LoadDriver(_T("IpFilterDriver"), _T("System32\\Drivers\\IpFltDrv.sys"), NULL, TRUE);
	//we don't deregister the driver at destructor
	filterDriver.SetRemovable(FALSE);
	helper.LoadDriver(_T("MyDriver"), NULL, NULL, TRUE);

	int order = 0;
	m_listRules.InsertColumn(order++,_T("Protocol"), LVCFMT_LEFT, 90);
	m_listRules.InsertColumn(order++,_T("Source IP"), LVCFMT_LEFT, 110);
	m_listRules.InsertColumn(order++,_T("Mask"), LVCFMT_LEFT, 110);
	m_listRules.InsertColumn(order++,_T("Port"), LVCFMT_LEFT, 80);
	m_listRules.InsertColumn(order++,_T("Dest IP"), LVCFMT_LEFT, 110);
	m_listRules.InsertColumn(order++,_T("Mask"), LVCFMT_LEFT, 110);
	m_listRules.InsertColumn(order++,_T("Port"), LVCFMT_LEFT, 80);

	order = 0;
	m_listPorts.InsertColumn(order++,_T("Port"), LVCFMT_LEFT, 70);
	m_listPorts.InsertColumn(order++,_T("Usage"), LVCFMT_LEFT, 90);
	m_listPorts.InsertColumn(order++,_T("State"), LVCFMT_LEFT, 90);

	order = 0;
	m_listWords.InsertColumn(order++,_T("Forbidden contents"), LVCFMT_LEFT, 100);

	m_portsManager.Init(&helper, &m_listPorts, &m_chartCtrl);

	UpdateData(false);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyFirewallDlg::OnPaint()
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
HCURSOR CMyFirewallDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyFirewallDlg::OnBnClickedBStart()
{
	m_started = true;
	GetDlgItem(IDC_BSTART)->EnableWindow(0);
	GetDlgItem(IDC_BSTOP)->EnableWindow(1);
	GetDlgItem(IDC_BRULE_ADD)->EnableWindow(1);
	GetDlgItem(IDC_BWORD_ADD)->EnableWindow(1);
	GetDlgItem(IDC_CHECK_MONITOR)->EnableWindow(1);
	GetDlgItem(IDC_CHECK_RULE)->EnableWindow(1);
	GetDlgItem(IDC_CHECK_WORD)->EnableWindow(1);
	GetDlgItem(IDC_CHECK_SESSION)->EnableWindow(1);
	helper.WriteIo(START_IP_HOOK, NULL, 0);
}

void CMyFirewallDlg::OnBnClickedBStop()
{
	m_started = false;
	GetDlgItem(IDC_BSTART)->EnableWindow(1);
	GetDlgItem(IDC_BSTOP)->EnableWindow(0);
	GetDlgItem(IDC_BRULE_ADD)->EnableWindow(0);
	GetDlgItem(IDC_BWORD_ADD)->EnableWindow(0);
	GetDlgItem(IDC_BRULE_DELETE)->EnableWindow(0);
	GetDlgItem(IDC_BWORD_DELETE)->EnableWindow(0);
	GetDlgItem(IDC_CHECK_MONITOR)->EnableWindow(0);
	GetDlgItem(IDC_CHECK_RULE)->EnableWindow(0);
	GetDlgItem(IDC_CHECK_WORD)->EnableWindow(0);
	GetDlgItem(IDC_CHECK_SESSION)->EnableWindow(0);
	helper.WriteIo(STOP_IP_HOOK, NULL, 0);
}

void CMyFirewallDlg::OnBnClickedBRuleAdd()
{
	UpdateData();

	IPFilter pf;

	pf.id = m_idRule++;

	if(m_protocolType == 0)
  		pf.protocol = 0;
	else if(m_protocolType == 1)
  		pf.protocol = 6;
	else
		pf.protocol = 17;

	char ascii[256];
	wcstombs( ascii, m_destIP, 256 );
	pf.destinationIp = inet_addr(ascii);
	wcstombs( ascii, m_sourceIP, 256 );
	pf.sourceIp = inet_addr(ascii);
	wcstombs( ascii, m_destMask, 256 );
	pf.destinationMask = inet_addr(ascii);
	wcstombs( ascii, m_sourceMask, 256 );
	pf.sourceMask = inet_addr(ascii);
	wcstombs( ascii, m_destPort, 256 );
	pf.destinationPort = htons(atoi(ascii));
	wcstombs( ascii, m_sourcePort, 256 );
	pf.sourcePort = htons(atoi(ascii));
	if( AddFilter(pf) )	//send the rule
	{
		int index = m_listRules.GetItemCount();
		int subIndex = 1;
		if(m_protocolType == 0)
			m_listRules.InsertItem(LVIF_TEXT|LVIF_PARAM, index, _T("All"), 0, 0, 0, pf.id);
		else if(m_protocolType == 1)
			m_listRules.InsertItem(LVIF_TEXT|LVIF_PARAM, index, _T("TCP"), 0, 0, 0, pf.id);
		else
			m_listRules.InsertItem(LVIF_TEXT|LVIF_PARAM, index, _T("UDP"), 0, 0, 0, pf.id);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_sourceIP, 0, 0, 0, 0, 0);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_sourceMask, 0, 0, 0, 0, 0);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_sourcePort, 0, 0, 0, 0, 0);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_destIP, 0, 0, 0, 0, 0);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_destMask, 0, 0, 0, 0, 0);
		m_listRules.SetItem(index, subIndex++, LVIF_TEXT, m_destPort, 0, 0, 0, 0, 0);
	}
}

BOOL CMyFirewallDlg::AddFilter(IPFilter &pf)
{
	DWORD result = helper.WriteIo(ADD_FILTER, &pf, sizeof(pf));

	if (result != DRV_SUCCESS) 
		return FALSE;
	else
		return TRUE;
}

BOOL CMyFirewallDlg::AddWord(WordFilter &wf)
{
	DWORD result = helper.WriteIo(ADD_WORD, &wf, sizeof(WordFilter));

	if (result != DRV_SUCCESS) 
		return FALSE;
	else
		return TRUE;
}

void CMyFirewallDlg::OnBnClickedBwordAdd()
{
	UpdateData();
	WordFilter wf;
	wf.id = m_idWord++;
	char ascii[10];
	wcstombs( ascii, m_word, 10 );
	strcpy(wf.word, ascii);
	if( AddWord(wf) )	//send the rule
		m_listWords.InsertItem(LVIF_TEXT|LVIF_PARAM, m_listWords.GetItemCount(), m_word, 0, 0, 0, wf.id);
	m_word = "";
	UpdateData(false);
}

void CMyFirewallDlg::OnLvnItemchangedListWord(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	GetDlgItem(IDC_BWORD_DELETE)->EnableWindow(1);
	*pResult = 0;
}

void CMyFirewallDlg::OnBnClickedBwordDelete()
{
	POSITION pos = m_listWords.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_listWords.GetNextSelectedItem(pos);
		USHORT id = m_listWords.GetItemData(nItem);
		if(helper.WriteIo(DEL_WORD, &id, sizeof(id))==DRV_SUCCESS){
			m_listWords.DeleteItem(nItem);
			GetDlgItem(IDC_BWORD_DELETE)->EnableWindow(0);
		}
	}
}

void CMyFirewallDlg::OnLvnItemchangedListRule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	GetDlgItem(IDC_BRULE_DELETE)->EnableWindow(1);
	*pResult = 0;
}

void CMyFirewallDlg::OnBnClickedBruleDelete()
{
	POSITION pos = m_listRules.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_listRules.GetNextSelectedItem(pos);
		USHORT id = m_listRules.GetItemData(nItem);
		if(helper.WriteIo(DEL_RULE, &id, sizeof(id))==DRV_SUCCESS){
			m_listRules.DeleteItem(nItem);
			GetDlgItem(IDC_BRULE_DELETE)->EnableWindow(0);
		}
	}
}

void CMyFirewallDlg::OnBnClickedCheck()
{
	UpdateData();
	SendSetting();
	if(m_portMonitor)
		SetTimer(100, update_interval, NULL);
	UpdateData(false);
}

void CMyFirewallDlg::UpdatePorts()
{
	m_portsManager.Update();
	m_nowSession = m_portsManager.SessionCount();
	m_total = m_portsManager.GetTotal();
	if(!m_exceed&&m_nowSession>m_maxSession)
	{
		m_exceed = true;
		if(m_sessionFilter)
			SendSetting();
	}
	else if(m_exceed&&m_nowSession<m_maxSession)
	{
		m_exceed = false;
		if(m_sessionFilter)
			SendSetting();
	}
}
void CMyFirewallDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_started && nIDEvent==100 && m_portMonitor)
    {
		UpdateData();
        SetTimer(100, update_interval, NULL);   // 타이머 다시 세팅..
		UpdatePorts();
		UpdateData(false);
    }
	CDialog::OnTimer(nIDEvent);
}

void CMyFirewallDlg::OnLvnItemchangedListPort(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_portsManager.ChangeSelected(pNMLV->iItem);
	*pResult = 0;
}

DWORD CMyFirewallDlg::SendSetting()
{
	FirewallSetting setting;
	setting.IP = m_IP;
	setting.Exceed = m_exceed;
	setting.IPFilter = m_IPFilter;
	setting.PortMonitor = m_portMonitor;
	setting.SessionFilter = m_sessionFilter;
	setting.WordFilter = m_wordFilter;
	return helper.WriteIo(SET_SETTING, &setting, sizeof(FirewallSetting));
}

void CMyFirewallDlg::GetLocalIP()        // local IP 획득
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				m_myIP = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}      
		WSACleanup();
	} 
}