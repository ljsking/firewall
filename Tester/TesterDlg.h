// TesterDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CTesterDlg 대화 상자
class CTesterDlg : public CDialog
{
// 생성입니다.
public:
	CTesterDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TESTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	BOOL AddFilter(IPFilter &pf);
	BOOL AddWord(WordFilter &wf);
	void UpdatePorts();


// 구현입니다.
protected:
	HICON m_hIcon;
	FilterHelper helper;
	FilterHelper filterDriver;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBStart();
	afx_msg void OnBnClickedBStop();
	afx_msg void OnBnClickedBRuleAdd();
	afx_msg void OnBnClickedBwordAdd();
	afx_msg void OnLbnSelcancelListWord();
	afx_msg void OnLbnSelchangeListWord();
	afx_msg void OnBnClickedBwordDelete();
	afx_msg void OnLvnItemchangedListRule(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBruleDelete();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedBupdate();
	afx_msg void OnHdnItemchangedListPort(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	CString m_myIP;
	CButton m_bStart;
	CButton m_bStop;
	CString m_sourceIP;
	CString m_sourceMask;
	CString m_sourcePort;
	CString m_destIP;
	CString m_destMask;
	CString m_destPort;
	CButton m_bRuleAdd;
	CButton m_bRuleDelete;
	CListCtrl m_listRules;
	CListBox m_listWords;
	CString m_word;
	CButton m_bWordAdd;
	CButton m_bWordDelete;
	int m_protocolType;
	FirewallSetting m_setting;
	CListCtrl m_listPorts;
	PortsManager m_portsManager;
	const int update_interval;
	CChartCtrl m_ChartCtrl;
};
