#if !defined(AFX_SNSCAN_H__20A4D962_7FC9_48AC_BCA6_1A75671DA4A0__INCLUDED_)
#define AFX_SNSCAN_H__20A4D962_7FC9_48AC_BCA6_1A75671DA4A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SNScan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SNScan dialog

class SNScan : public CDialog
{
// Construction
public:
	void drawResult2(int color, char *str);
	SNScan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SNScan)
	enum { IDD = IDD_SNScan };
	CStatic	m_MACBOX;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SNScan)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SNScan)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNSCAN_H__20A4D962_7FC9_48AC_BCA6_1A75671DA4A0__INCLUDED_)
