// 无线测试Dlg.h : header file
//

#if !defined(AFX_DLG_H__8FC82B30_79C9_4B05_A4E3_1D841AF6C85A__INCLUDED_)
#define AFX_DLG_H__8FC82B30_79C9_4B05_A4E3_1D841AF6C85A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "afxcmn.h"

#include "configure.h"
#include "ThroughputTest.h"
#include "ADOConn.h"
#include "winsock.h"
#include "SNScan.h"

// [liguoliang start]
#include "diagDeviceInfo.h"
#include "httpDeviceInfo.h"
#include "tddpDeviceInfo.h"
#include "sqlite3.h"
#include "testItems.h"
#include "fmifs.h"
#include "icmp.h"
#include "device.h"
// [liguoliang end]

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

class CMyDlg : public CDialog
{
// Construction
public:
	CMyDlg(CWnd* pParent = NULL);	// standard constructor

	void drawMACResult(int color, char *str);
	void drawResult(int color, char *str);
	bool readThroughputConfigure();
	bool readDUTConfigure();

	// [liguoliang start]
	int initialOpera(char *dev_name);
	void initialUIVar();
	void initialDevType();
	void checkOpera();
	void saveIniFile();

	void updateCurState(CString str);
	void updateResult(char *str, int color);
	void updatePassPic(int visible);
	void updateFailPic(int visible);
	void changeTestButton(int begin_or_stop);
	void updateDevicInfo();

	void updateWiFiBandInfo();

	CDC *pDC;
	CString strCurState;  // 测试过程中显示在主界面下面状态信息控件的内容
	HWND hWnd;            // 指向主界面的窗口句柄
	// [liguoliang end]


// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_MY_DIALOG };
	CStatic	m_DUTMAC;
	CButton	m_Amend;
	CButton	m_Confirm;
	CComboBox	m_Xianbie;
	CEdit	m_Number;
	CIPAddressCtrl	m_RemoteIP;
	CEdit	m_TestFilename;
	CIPAddressCtrl	m_DUTIP;
	CEdit	m_PassAvg;
	CButton	m_TestThroughput;
	CStatic	m_CurState;
	CStatic	m_Avg;
	CStatic	m_Result;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrose();
	afx_msg void OnTestthroughput();
	afx_msg void OnReadrecord();
	afx_msg void OnConfirm();
	afx_msg void OnAmend();
	afx_msg void OnPing();
	afx_msg void OnReset();
	afx_msg void OnFeedbk();
	afx_msg void OnTypeSyn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_RecordList;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
//	afx_msg void OnClose();
	afx_msg void OnBnClickedBopenwifi();
	afx_msg void OnClose();
	afx_msg void OnBnClickedChangeBand();
	afx_msg void OnBnClickedLteReset();
	afx_msg void OnCbnSelchangeXianbie();
    CEdit m_PassAvg_5g;
    afx_msg void OnEnChangeNumber2();
    void OnChangeAutoConnect();
    CEdit m_MacAddress;
    afx_msg void OnStnClicked3gmodemlabel2();
    afx_msg void OnBnClickedAudioListen();
    CEdit m_Station;
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

struct VersionInfos {
	CString product_name;
	CString product_ver;
	CString company_name;
	CString copyright_info;
};

void _get_version_info(struct VersionInfos &v);


#endif // !defined(AFX_DLG_H__8FC82B30_79C9_4B05_A4E3_1D841AF6C85A__INCLUDED_)
