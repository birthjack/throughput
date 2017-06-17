#pragma once


// CPassDlg dialog

class CPassDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPassDlg)

public:
	CPassDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPassDlg();

// Dialog Data
	enum { IDD = IDD_PASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
