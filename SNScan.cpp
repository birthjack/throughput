// SNScan.cpp : implementation file
//

#include "stdafx.h"
#include "WirelessTest.h"
#include "SNScan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char sn[15];
extern CString DUTMAC;
/////////////////////////////////////////////////////////////////////////////
// SNScan dialog


SNScan::SNScan(CWnd* pParent /*=NULL*/)
	: CDialog(SNScan::IDD, pParent)
{
	//{{AFX_DATA_INIT(SNScan)
	//}}AFX_DATA_INIT
}


void SNScan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SNScan)
	DDX_Control(pDX, IDC_MACBOX, m_MACBOX);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SNScan, CDialog)
	//{{AFX_MSG_MAP(SNScan)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SNScan message handlers

BOOL SNScan::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
	{
		EndDialog(1);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

int SNScan::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DoModal();
}


void SNScan::drawResult2(int color, char *str)
{
	CFont ResultFont;	
	CStatic* ResultHwnd = (CStatic*)GetDlgItem(IDC_MACBOX);
	ResultHwnd->RedrawWindow();
	CDC *pDC = ResultHwnd->GetDC();
	ResultFont.CreatePointFont(350,"Arial",pDC);
	pDC->SelectObject(&ResultFont);
	pDC->SetBkColor(RGB(0,255,255));
	pDC->SetTextColor(color);
	pDC->TextOut(0, 0, str);
}
