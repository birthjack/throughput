// 无线测试Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "WirelessTest.h"
#include "WirelessTestDlg.h"
#include "WSDataTransCpp.h"
#include "XMLConfig.h"
#include <WinVer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyDlg * g_Dlg = NULL;   // 指向主界面的指针
Device * product = NULL; // 产品类的父类对象指针，根据实际产品类型指向不同的子类对象
CWSDataTransCpp * mic = NULL;
// 路由配置 route ADD 172.0.0.0 MASK 255.0.0.0 172.28.130.1 METRIC 1 -p
char * devid = NULL;


static struct VersionInfos _version_infos;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString title;
	title.Format("关于 %s V%s", _version_infos.product_name, _version_infos.product_ver);
	this->SetWindowText(title);

	CString pro_info;
	pro_info.Format("%s", _version_infos.company_name);
	((CStatic *)this->GetDlgItem(IDC_MAN_INFO))->SetWindowText(pro_info);

	CString copy_info;
	copy_info.Format("%s", _version_infos.copyright_info);
	((CStatic *)this->GetDlgItem(IDC_COPY_INFO))->SetWindowText(copy_info);

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDlg)
    DDX_Control(pDX, IDC_AMEND, m_Amend);
    DDX_Control(pDX, IDC_CONFIRM, m_Confirm);
    DDX_Control(pDX, IDC_XIANBIE, m_Xianbie);
    DDX_Control(pDX, IDC_NUMBER, m_Number);
    DDX_Control(pDX, IDC_REMOTEIP, m_RemoteIP);
    DDX_Control(pDX, IDC_TESTFILENAME, m_TestFilename);
    DDX_Control(pDX, IDC_DUTIP, m_DUTIP);
    DDX_Control(pDX, IDC_PASSAVG, m_PassAvg);
    DDX_Control(pDX, IDC_TESTTHROUGHPUT, m_TestThroughput);
    DDX_Control(pDX, IDC_CURSTATE, m_CurState);
    DDX_Control(pDX, IDC_AVG, m_Avg);
    DDX_Control(pDX, IDC_RESULT, m_Result);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_LIST1, m_RecordList);
    DDX_Control(pDX, IDC_PASSAVG_5g, m_PassAvg_5g);
    DDX_Control(pDX, IDC_AUTO_CONNECT, m_MacAddress);
    DDX_Control(pDX, IDC_STATION, m_Station);
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROSE, OnBrose)
	ON_BN_CLICKED(IDC_TESTTHROUGHPUT, OnTestthroughput)
	ON_BN_CLICKED(IDC_READRECORD, OnReadrecord)
	ON_BN_CLICKED(IDC_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_AMEND, OnAmend)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_FEEDBK, OnFeedbk)
	ON_BN_CLICKED(IDC_TYPESYN, OnTypeSyn)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BOPENWIFI, &CMyDlg::OnBnClickedBopenwifi)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHANGE_BAND, &CMyDlg::OnBnClickedChangeBand)
	ON_BN_CLICKED(IDC_LTE_RESET, &CMyDlg::OnBnClickedLteReset)
//    ON_EN_CHANGE(IDC_AUTO_CONNECT, &CMyDlg::OnChangeAutoConnect)
    ON_BN_CLICKED(IDC_AUDIO_LISTEN, &CMyDlg::OnBnClickedAudioListen)
END_MESSAGE_MAP()


BOOL CMyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_SPACE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		CMyDlg::OnChangeAutoConnect();
	return CDialog::PreTranslateMessage(pMsg);
}


void CMyDlg::OnOK()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}


void CMyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (strcmp(StrResult, _T("  合格")) == 0)
		{
			drawResult(RGB(0, 255, 0), StrResult);
		}
		else if (strcmp(StrResult, _T("不合格")) == 0)
		{
			drawResult(RGB(255, 0, 0), StrResult);
		}
		else if (strcmp(StrResult, _T("MAC未烧录")) == 0)
		{
			drawResult(RGB(255, 0, 0), StrResult);
		}
		else
		{
			drawResult(RGB(0, 0, 0), StrResult);
		}
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers


// 主界面的初始化工作以及程序的一些配置参数相关的初始化工作
BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_Dlg = this; //定义对话框的全局对象

	_get_version_info(_version_infos);
	CString title;
	title.Format("%s V%s", _version_infos.product_name, _version_infos.product_ver);
	this->SetWindowText(title);

	//进行winsock的初始化，必须，非常重要，并且需要在关闭程序时释放
	WSADATA wsaData;
	WORD version=MAKEWORD(2,0);
	WSAStartup(version,&wsaData);

	// [liguoliang start] 载入相关dll函数操作以及初始化全局变量和界面显示内容
	if( !LoadDLLFunctions()) {
		// 显示在界面上
		MessageBox("载入相关dll失败，会导致格式化和ping操作失败!","提示",MB_OK);
	}

	readDUTConfigure();
	readThroughputConfigure();

	// 初始化配置参数
	initialGlobalConfigure();

	// 初始化界面内容
	initialUIVar();

	if(!initialCOM())
	{
		MessageBox("初始化串口句柄失败，会导致屏蔽箱操作失败!","提示",MB_OK);
	}
	// [liguoliang end]

    // Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// 其他初始化
	CStatic* ResultHwnd = (CStatic*)g_Dlg->GetDlgItem(IDC_RESULT);
	//ResultHwnd->RedrawWindow();
	pDC = ResultHwnd->GetDC();

	// 增加线程锁
	hMutex_mic = CreateMutex(NULL,FALSE,"mic");
	hMutex_iperf = CreateMutex(NULL,FALSE,"iperf");

	// 创建检测设备连接状态线程
	runState.pingDUT = STOP_PING_DUT;
	if (0 == WIFI_CONTROL)
	{
		connDUTThread = CreateThread(NULL, 0, connectDUTThread, NULL, 0, NULL);
	}

	if (1 == CON_MES_ENABLE)
	{
		connMICThread = CreateThread(NULL, 0, connectMICThread, NULL, 0, NULL);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// 关闭程序时将用户输入的配置内容保存到config文件中
void CMyDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	runState.pingDUT = CLOSE_PING_DUT;

	// 调用保存配置文件的函数
	saveIniFile();

	// 释放载入的fmifs.dll
	if (fmisfDll!=NULL)
	{
		FreeLibrary(fmisfDll);
		fmisfDll = NULL;
	}

	// 释放载入的icmp.dll
	if (icmpDll!=NULL)
	{
		FreeLibrary(icmpDll);
		icmpDll = NULL;
	}

	// 释放product产品类对象的指针
	if (product != NULL)
	{
		delete product;
		product = NULL;
	}

	// 释放句柄
	if (connDUTThread != NULL)
	{
		CloseHandle(connDUTThread);
		connDUTThread = NULL;
	}
	if (wifiThread != NULL)
	{
		CloseHandle(wifiThread);
		wifiThread = NULL;
	}
	if (connMICThread != NULL)
	{
        CloseHandle(connMICThread);
        connMICThread = NULL;
	}

	if (hMutex_mic != NULL)
	{
		CloseHandle(hMutex_mic);
        hMutex_mic = NULL;
	}

	if (hMutex_iperf != NULL)
	{
		CloseHandle(hMutex_iperf);
        hMutex_iperf = NULL;
	}


	// 释放winsock资源
	WSACleanup();

	// 其他资源释放
	CStatic* ResultHwnd = (CStatic*)g_Dlg->GetDlgItem(IDC_RESULT);
	ResultHwnd->ReleaseDC(pDC);

	CDialog::OnClose();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 以下为各按钮控件单击响应函数
////////////////////////////////////////////////////////////////////////////////


//选择机型"确定"按钮
void CMyDlg::OnConfirm()
{
	// TODO: Add your control notification handler code here
	m_Number.GetWindowText(nb,10);
	m_Station.GetWindowText(station_number,15);

	CComboBox* combox1 = (CComboBox*)this->GetDlgItem(IDC_XIANBIE);
	CON_CurSel = combox1->GetCurSel();

	((CButton *)this->GetDlgItem(IDC_AUTO_CONNECT))->EnableWindow(TRUE);

	if (1 == WIFI_CONTROL)
	{
		int open_wifi = allDev[CON_CurSel].close_wifi;
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);

		if (1 == open_wifi)
		{
			GetDlgItem(IDC_BOPENWIFI)->EnableWindow(1);
		}
		return;
	}

	if (0 == strcmp(nb,"") || 0 == strcmp(station_number,""))
	{
		MessageBox("请输入工号和工位","提示",MB_OK);
		return;
	}

	m_Xianbie.GetWindowText(CON_model,MAX_DEVICE_NAME_LEN);


	// 查询数据库或者配置文件判断需要操作的功能
	if (FALSE == initialOpera(CON_model))
	{
		return;
	}

	// 初始化设备类型
	initialDevType();

	//更新默认IP
	unsigned long dwAddress;
	dwAddress = inet_addr(CON_DUTIP);
	dwAddress = ntohl(dwAddress);
	m_DUTIP.SetAddress(dwAddress);


	// 固定输入工号、选择机型、确定这三个控件
	(CEdit*)g_Dlg->GetDlgItem(IDC_NUMBER)->EnableWindow(FALSE);//捕获搜索按钮指针
	(CEdit*)g_Dlg->GetDlgItem(IDC_STATION)->EnableWindow(FALSE);
	(CComboBox*)g_Dlg->GetDlgItem(IDC_XIANBIE)->EnableWindow(FALSE);//捕获搜索按钮指针
	(CButton*)g_Dlg->GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);

	// 如果是 LTE 设备，则启用band控件
	if (CON_DevType == DEVICE_LTE_MIFI || CON_DevType == DEVICE_LTE_MSM8909) {
		((CStatic *)g_Dlg->GetDlgItem(IDC_BANDLABEL))->EnableWindow(TRUE);
		((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->EnableWindow(TRUE);
	}

	runState.pingDUT = BEGIN_PING_DUT;

}


//选择机型"修改"按钮
void CMyDlg::OnAmend()
{
	// TODO: Add your control notification handler code here
	runState.pingDUT = STOP_PING_DUT;
	runState.stop = TRUE;

	(CEdit*)g_Dlg->GetDlgItem(IDC_NUMBER)->EnableWindow(TRUE);//捕获搜索按钮指针
	(CEdit*)g_Dlg->GetDlgItem(IDC_STATION)->EnableWindow(TRUE);//捕获搜索按钮指针
	(CComboBox*)g_Dlg->GetDlgItem(IDC_XIANBIE)->EnableWindow(TRUE);//捕获搜索按钮指针
	(CButton*)g_Dlg->GetDlgItem(IDC_CONFIRM)->EnableWindow(TRUE);
	(CButton*)g_Dlg->GetDlgItem(IDC_PING)->EnableWindow(TRUE);//捕获搜索按钮指针

	((CButton*)GetDlgItem(IDC_CFORMAT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_C3GMODEM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CWAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CPIN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CRST))->SetCheck(0);
	//((CButton*)GetDlgItem(IDC_CVER))->SetCheck(0);
	GetDlgItem(IDC_BOPENWIFI)->EnableWindow(0);

	((CButton *)g_Dlg->GetDlgItem(IDC_BANDLABEL))->EnableWindow(FALSE);
	((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->SetWindowText("暂无信息");
	((CButton *)g_Dlg->GetDlgItem(IDC_DEVBAND))->EnableWindow(FALSE);
	((CButton *)g_Dlg->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(FALSE);
	((CButton *)g_Dlg->GetDlgItem(IDC_LTE_RESET))->EnableWindow(FALSE);
}


//"PING"按钮
void CMyDlg::OnPing()
{
	CString ip;
	GetDlgItemText(IDC_REMOTEIP, ip);
	strcpy(CON_remoteIP, ip);
	CString ping;
	ping.Format("ping -t %s",CON_remoteIP);
	WinExec(ping,SW_SHOW);
}


//测试文件"浏览"按钮
void CMyDlg::OnBrose()
{
	// TODO: Add your control notification handler code here
	CFileDialog my(TRUE, NULL, NULL, OFN_NOCHANGEDIR, "测试文件(*.tst)|*.tst|所有文件(*.*)|*.*||");

	if (IDCANCEL == my.DoModal())
		return;

	CFileFind find;

	if (0 != find.FindFile(my.GetPathName(), 0))
	{
		strcpy(CON_testFilename, my.GetPathName());
		m_TestFilename.SetWindowText(my.GetPathName());
	}
	else
	{
		::AfxMessageBox("不存在该文件");
	}
}

//"开始测试"按钮
void CMyDlg::OnTestthroughput()
{
	// TODO: Add your control notification handler code here
	CString strButton;

	//static int ht40 = FALSE;
	int channel = 0;  //设置信道
	int bandwidth = 2; //设置带宽

	/* 先判断是否输入了工号、工号 */
	CEdit* m_Number = (CEdit*)g_Dlg->GetDlgItem(IDC_NUMBER);
	CEdit* m_Station = (CEdit*)g_Dlg->GetDlgItem(IDC_STATION);
	BOOL i = m_Number->IsWindowEnabled()|m_Station->IsWindowEnabled();
	if (i != 0)
	{
		MessageBox("请输入工号和工位，并点击确定按钮！","提示",MB_OK);
		return;
	}

	GetDlgItemText(IDC_TESTTHROUGHPUT, strButton);
   	if (strButton == _T("开始测试"))
	{
		// 根据复选框再次判断需要操作的功能
		checkOpera();

		if (CON_TEST_TYPE == IPERF_TX_TEST)
		{
			runState.iperfStatus = IPERF_START_PC;
		}
		else if (CON_TEST_TYPE == IPERF_RX_TEST)
		{
			runState.iperfStatus = IPERF_START_DUT;
		}
		else
		{
			runState.iperfStatus = IPERF_STOP;
		}

		runState.stop = FALSE;
		readDUTConfigure();
		readThroughputConfigure();

		if (DEVICE_LTE_CMCC == CON_DevType)
		{
			g_Dlg->strCurState = "";

			if (FALSE == CON_HT40)
			{
				g_Dlg->updateResult("正在设置带宽", RGB(0,0,0));
				g_Dlg->updateCurState(">>设置信道和带宽");
				runState.pingDUT = PAUSE_PING_DUT;
				CON_HT40= fixBandTo40(channel, bandwidth);

				//该时间是为了确保带宽设置生效，但由于驻留时间较长会出现卡顿的现象，
				//测试发现去掉该时间也完全能设置生效，故暂时去掉
				//Sleep(8000);

				runState.pingDUT = BEGIN_PING_DUT;

			}
			if (FALSE == CON_HT40)
			{
				g_Dlg->updateResult("带宽设置失败", RGB(0,0,0));
				return;
			}
			else
			{
				g_Dlg->updateResult("带宽设置成功", RGB(0,0,0));
			}
		}

		changeTestButton(BEGIN_TEST);
		CreateThread(NULL, 0, throughputTest, NULL, 0, NULL); //调用线程开始测试吞吐量
	}
	else if (strButton == _T("停止测试"))
	{
		runState.stop = TRUE;

		changeTestButton(STOP_TEST);

		return;
	}
}


//"查看记录"按钮
void CMyDlg::OnReadrecord()
{
	// TODO: Add your control notification handler code here
	CString records;
	CString tmp;

	records.Format("序号   时间            MAC地址         是否合格      测试结果\r\n");
	if (counts < recordCount)
	{
		for (int i = 0; i < counts; i++)
		{
			tmp.Format(" %02d.  ", i+1);
			records.Insert(10000, tmp);
			records.Insert(10000, testRecord[(begRecordPos + i) % recordCount]);
			records.Insert(10000, "\n");
		}
	}
	else
	{
		for (int i = 0; i < recordCount; i++)
		{
			tmp.Format(" %02d.  ", i+1);
			records.Insert(10000, tmp);
			records.Insert(10000, testRecord[(begRecordPos + i) % recordCount]);
			records.Insert(10000, "\r\n");
		}
	}

	MessageBox(records, "测试记录", MB_OK);
}


//"清零"按钮
void CMyDlg::OnReset()
{
	// TODO: Add your control notification handler code here
	passtimes=0;
	failtimes=0;
	passpro=0.00;

    CStatic* edit1=(CStatic*)GetDlgItem(IDC_PASSTIMES);
	edit1->SetWindowText("0");

	CStatic* edit2=(CStatic*)GetDlgItem(IDC_FAILTIMES);
	edit2->SetWindowText("0");

	CStatic* edit3=(CStatic*)GetDlgItem(IDC_PASSPRO);
	edit3->SetWindowText("0.00%");
}


//"MAC错误"按钮
void CMyDlg::OnFeedbk()
{
	CString sqlset;
	ADOConn m_AdoConn1;

	if (CON_DBConnect == 0)
	{
		AfxMessageBox("需要连接数据库，请更改配置文件参数!");
		return;
	}

	sqlset.Format("select * from ThroughputLog where tl_pid = %ld and tl_result = 1 \
					order by tl_id desc",product_id);
	_RecordsetPtr m_pRecordset;
	m_pRecordset=m_AdoConn1.GetRecordSet((_bstr_t)sqlset);

	try
	{
		if(m_AdoConn1.m_pRecordset->adoEOF!=0)
		{
			AfxMessageBox("信息上传错误！未找到测试记录！！");
		}
		else
		{
			m_pRecordset->PutCollect("tl_result", (_variant_t)0);
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"实际MAC地址与锡标MAC地址不一致");
			m_pRecordset->Update();
			m_AdoConn1.ExitConnect();

			AfxMessageBox ("信息提交成功！谢谢！");
		}
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");
	}

	changeDBProductStatus(0);
}

//"机型同步"按钮
void CMyDlg::OnTypeSyn()
{
	// TODO: Add your control notification handler code here
	//****************************自动获取服务器机型********************************
	CString sqlset;
	CString device_config;
    char temp[5];

	if (CON_DBConnect == 0)
	{
		AfxMessageBox("需要连接数据库，请更改配置文件参数!");
		return;
	}

	ADOConn m_AdoConn1;

	// 查询机型及吞吐量配置信息
	sqlset.Format("select * from DeviceModel,ThroughputConfig where t_model=d_id order by d_id desc");
	//sqlset.Format("select d_name from DeviceModel order by d_id desc");

	_RecordsetPtr m_pRecordset;
	m_pRecordset=m_AdoConn1.GetRecordSet((_bstr_t)sqlset);

	try
	{
		int num = 0;
		while(!m_pRecordset->adoEOF)
		{
			num++;
			jx.Format("机型%d",num);
			int d_wifi = m_pRecordset->GetCollect("d_wifi");
			device_config.Format("%s,%d", (char*)(_bstr_t)m_pRecordset->GetCollect("d_name"),
									d_wifi);
			WritePrivateProfileString("机型信息",jx, device_config,JIXING);
			m_pRecordset->MoveNext();

			if (num==50)
			{
				break;
			}
		}

		sprintf(temp,"%d",num);
		WritePrivateProfileString("机型信息","机型总数", temp,JIXING);
		AfxMessageBox("机型信息更新完毕！重新启动软件生效！");
		m_AdoConn1.ExitConnect();

	}
	catch(...)
	{
		AfxMessageBox("数据请求失败，无法获取服务器机型新型！");
	}
}

//"开启本端设备WIFI"按钮
void CMyDlg::OnBnClickedBopenwifi()
{
	// TODO: Add your control notification handler code here
	CString strButton;
	GetDlgItemText(IDC_BOPENWIFI, strButton);
	if (strButton == _T("开启本端\n设备WiFi"))
	{
		runState.stop = FALSE;
		((CButton*)GetDlgItem(IDC_BOPENWIFI))->SetWindowText("停止开启\n设备WiFi");

		wifiThread = CreateThread(NULL, 0, openWiFiThread, NULL, 0, NULL); //调用线程开始测试吞吐量
	}
	else if (strButton == _T("停止开启\n设备WiFi"))
	{
		runState.stop = TRUE;
		g_Dlg->updateResult("", RGB(0,0,0));
		((CButton*)GetDlgItem(IDC_BOPENWIFI))->SetWindowText("开启本端\n设备WiFi");
	}
}


// 切换Band
void CMyDlg::OnBnClickedChangeBand()
{
	runState.pingDUT = STOP_PING_DUT;

	((CStatic *)this->GetDlgItem(IDC_DEVBAND))->SetWindowText("切换中...");
	((CButton *)this->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(FALSE);
	((CButton *)this->GetDlgItem(IDC_LTE_RESET))->EnableWindow(FALSE);

	enum WiFiBand band = product->getWiFiBand();
	switch (band) {
	case BAND_2_GHZ:
		product->setWiFiBand(BAND_5_GHZ);
		break;
	case BAND_5_GHZ:
		product->setWiFiBand(BAND_2_GHZ);
		break;
	default:
		break;
	}

	Sleep(8000);
	runState.pingDUT = BEGIN_PING_DUT;
}

void CMyDlg::OnBnClickedLteReset()
{
	runState.pingDUT = STOP_PING_DUT;

	((CStatic *)this->GetDlgItem(IDC_DEVBAND))->SetWindowText("重置中...");
	((CButton *)this->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(FALSE);
	((CButton *)this->GetDlgItem(IDC_LTE_RESET))->EnableWindow(FALSE);

	product->reset();

	Sleep(8000);
	runState.pingDUT = BEGIN_PING_DUT;
}


void _get_version_info(struct VersionInfos &v)
{
	CString ret;
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *TranslationPtr;
	DWORD res_size, para;
	TCHAR *res_buf = NULL;
	UINT ver_info_size;
	TCHAR ver_value[128];
	TCHAR *info_ptr;
	sprintf(ver_value, "%s.exe", AfxGetAppName());
	res_size = GetFileVersionInfoSizeA(ver_value, &para);
	if (res_size > 0) {
		res_buf = new TCHAR[res_size];
		if (res_buf == NULL)
			goto bad_mem;

		if (GetFileVersionInfo(ver_value, NULL, res_size, res_buf) == NULL)
			goto bad;

		//VerQueryValue(res_buf, );
		if(VerQueryValue(res_buf, "VarFileInfo\\Translation",
						(LPVOID*)&TranslationPtr, &ver_info_size) == NULL)
			goto bad;

		sprintf(ver_value, "\\StringFileInfo\\%04x%04x\\ProductVersion",
				TranslationPtr[0].wLanguage, TranslationPtr[0].wCodePage);
		if(VerQueryValue(res_buf, ver_value, (LPVOID*)&info_ptr, &ver_info_size) == NULL)
			goto bad;

		if (strlen(info_ptr) > 0)
			v.product_ver = CString(info_ptr);

		sprintf(ver_value, "\\StringFileInfo\\%04x%04x\\ProductName",
				TranslationPtr[0].wLanguage, TranslationPtr[0].wCodePage);
		if(VerQueryValue(res_buf, ver_value, (LPVOID*)&info_ptr, &ver_info_size) == NULL)
			goto bad;

		if (strlen(info_ptr) > 0)
			v.product_name = CString(info_ptr);

		sprintf(ver_value, "\\StringFileInfo\\%04x%04x\\LegalCopyright",
				TranslationPtr[0].wLanguage, TranslationPtr[0].wCodePage);
		if(VerQueryValue(res_buf, ver_value, (LPVOID*)&info_ptr, &ver_info_size) == NULL)
			goto bad;

		if (strlen(info_ptr) > 0)
			v.copyright_info = CString(info_ptr);

		sprintf(ver_value, "\\StringFileInfo\\%04x%04x\\CompanyName",
				TranslationPtr[0].wLanguage, TranslationPtr[0].wCodePage);
		if(VerQueryValue(res_buf, ver_value, (LPVOID*)&info_ptr, &ver_info_size) == NULL)
			goto bad;

		if (strlen(info_ptr) > 0)
			v.company_name = CString(info_ptr);
	}

bad:
	delete[] res_buf;
bad_mem:
	return;
}

void CMyDlg::OnChangeAutoConnect()
{
    char mac[30];
    char cmd[256]={};
    int mac_length = 12;
    int ssid_prefix_hex_len = 28;
    int mac_suffix_length = product->getMacSuffix();

    m_MacAddress.GetWindowText(mac, 30);

    if (strlen(mac) == mac_length)
    {
        CXMLConfig xmlConfig("connect.xml");

        string ssid_pre = xmlConfig.getXmlString("/ns:WLANProfile/ns:name");
        string ssid_hex = xmlConfig.getXmlString("/ns:WLANProfile/ns:SSIDConfig/ns:SSID/ns:hex");
        string ssid;

        // generate ssid
        if (ssid_pre.length() >= mac_suffix_length && strlen(mac) >= mac_suffix_length)
        {
            ssid = ssid_pre.substr(0, ssid_pre.length() - mac_suffix_length) + &mac[strlen(mac) - mac_suffix_length];
            // generate ssid with hex type
            ssid_hex = ssid_hex.substr(0, ssid_prefix_hex_len) + xmlConfig.bytestohexstring(&mac[strlen(mac) - mac_suffix_length], mac_suffix_length);
        }
        else
        {
            return;
        }

        xmlConfig.writeXmlString(ssid,"/ns:WLANProfile/ns:name");
        xmlConfig.writeXmlString(ssid,"/ns:WLANProfile/ns:SSIDConfig/ns:SSID/ns:name");
        xmlConfig.writeXmlString(ssid_hex,"/ns:WLANProfile/ns:SSIDConfig/ns:SSID/ns:hex");
        if(!xmlConfig.saveConfigFile())
        {
            return;
        }

        this->updateResult("正在连接DUT", RGB(0,0,0));
        sprintf(cmd, "netsh wlan connect name=%s ssid=%s", ssid_pre.c_str(), ssid.c_str());
        system("netsh wlan disconnect");
        system("netsh wlan delete profile *");
        Sleep(1000);
        system("netsh wlan add profile filename=connect.xml");
        system(cmd);

        m_MacAddress.SetWindowText("");
    }
    else if (strlen(mac) > mac_length)
    {
        this->updateResult("MAC长度过长", RGB(255,0,0));
        m_MacAddress.SetWindowText("");
    }
    return;
}



void CMyDlg::OnBnClickedAudioListen()
{
	audioListen();
}
