/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   CMyDlg.cpp

  Description: 将CMyDlg类中的非响应控件的函数分离实现在该文件中，使得无线测试Dlg.cpp代码简练

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-17, liguoliang      create file.
*******************************************************************************/
#include <windows.h>

#include "StdAfx.h"
#include "WirelessTestDlg.h"


#include <stdlib.h>
#include "md5.h"
#include <string.h>

#include <afxinet.h>
//#include <WinInet.h>



extern CMyDlg * g_Dlg;   // 指向主界面的指针
extern Device * product; // 产品类的父类对象指针，根据实际产品类型指向不同的子类对象


bool CMyDlg::readDUTConfigure()
{
	unsigned long  dwAddress;
	CString strAdd;
	m_DUTIP.GetAddress(dwAddress);
	strAdd.Format("%d.%d.%d.%d", (dwAddress & 0xff000000) >> 24, (dwAddress & 0x00ff0000) >> 16, (dwAddress & 0x0000ff00) >> 8, (dwAddress & 0x000000ff));
	strcpy(CON_DUTIP, strAdd);
	return TRUE;
}

bool CMyDlg::readThroughputConfigure()
{
	char tmp[10];
	unsigned long dwAddress;
	CString strAdd;

	m_RemoteIP.GetAddress(dwAddress);
	strAdd.Format("%d.%d.%d.%d", (dwAddress & 0xff000000) >> 24, (dwAddress & 0x00ff0000) >> 16, (dwAddress & 0x0000ff00) >> 8, (dwAddress & 0x000000ff));
	strcpy(CON_remoteIP, strAdd);

	m_TestFilename.GetWindowText(CON_testFilename, MAX_PATH);

	return TRUE;
}

void CMyDlg::drawResult(int color, char *str)
{
	CFont ResultFont;
	(CStatic*)g_Dlg->GetDlgItem(IDC_RESULT)->RedrawWindow();

	if (NULL != strstr(str, "PASS"))
	{
		ResultFont.CreatePointFont(440,"Arial",pDC);
		pDC->SetBkColor(RGB(0,200,0));
		pDC->SetTextColor(color);
		pDC->SelectObject(&ResultFont);
		pDC->TextOut(5, 5, str);
	} else if (NULL != strstr(str, "FAIL"))
	{
		ResultFont.CreatePointFont(440,"Arial",pDC);
		pDC->SetBkColor(RGB(230,0,0));
		pDC->SetTextColor(color);
		pDC->SelectObject(&ResultFont);
		pDC->TextOut(5, 5, str);
	} else
	{
		ResultFont.CreatePointFont(360,"Arial",pDC);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(color);
		pDC->SelectObject(&ResultFont);
		pDC->TextOut(0, 10, str);
	}
	ResultFont.DeleteObject();
}

/*******************************************************************************
  Function:    drawMACResult
  Description: 针对显示MAC的控件指定显示格式
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
void CMyDlg::drawMACResult(int color, char *str)
{
	CFont ResultFont;
	CStatic* ResultHwnd = (CStatic*)g_Dlg->GetDlgItem(IDC_DUTMAC);
	ResultHwnd->RedrawWindow();
	CDC *pDC = ResultHwnd->GetDC();
	ResultFont.CreatePointFont(380,"Arial",pDC);
	pDC->SelectObject(&ResultFont);
	pDC->SetBkColor(RGB(0,200,0));
	pDC->SetTextColor(color);
	pDC->TextOut(0, 0, str);
	ResultFont.DeleteObject();
	ResultHwnd->ReleaseDC(pDC);
}

/*******************************************************************************
  Function:    initialDevType
  Description: 初始化设备类型，每次测试获取设备信息前需要先调用该函数
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
void CMyDlg::initialDevType()
{
	CComboBox* combobox1=(CComboBox*)GetDlgItem(IDC_XIANBIE);
	CON_CurSel = combobox1->GetCurSel();

	// 先释放之前创建的对象
	if (product != NULL)
	{
		delete product;
		product = NULL;
	}

	//*********************这一步很重要，根据不同机型类型生成对应的不同子类**************
	//在后面涉及调用的Device父类函数的时候，都是根据实际指向的子类对象调用不同子类的函数
	switch(CON_DevType)
	{
	case DEVICE_7S:
		product = new Device_7s();
		break;
	case DEVICE_8S:
		product = new Device_8s();
		break;
	case DEVICE_ROUTER_AND_3G:
		product = new Device_RouterAnd3G();
		break;
	case DEVICE_ROUTER:
		product = new Device_Router();
		break;
	case DEVICE_LTE_MIFI:
	case DEVICE_LTE_MSM8909:
		product = new Device_lte();
		break;
	case DEVICE_LTE_CMCC:
		product = new Device_lte_cmcc();
		break;
	case DEVICE_TPWEARABLE:
		product = new DeviceTPWearable();
		break;
	case DEVICE_CARDV:
		product = new DeviceCarDv();
		break;
	}
	product->initial = 0;

	// 初始化productInfo结构变量的内容
	strcpy(productDBInfo.product_name, CON_model);
	strcpy(productDBInfo.product_mac, "N/A");
	strcpy(productDBInfo.product_imei,"N/A");
	strcpy(productDBInfo.product_meid,"N/A");
	strcpy(productDBInfo.product_pin,"N/A");
	strcpy(productDBInfo.product_sn,"N/A");
}

/*******************************************************************************
  Function:    initialUIVar
  Description: 初始化界面中的显示的变量
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
void CMyDlg::initialUIVar()
{
	CString str;
	unsigned long dwAddress;

	//***************************初始化界面中的产品列表:star************************
	CComboBox* combobox1=(CComboBox*)GetDlgItem(IDC_XIANBIE);
	char d_info[MAX_DEVINFO_SIZE];
	for( int k=1;k<=product_total_num;k++)
	{
		jx.Format("机型%d",k);

		GetPrivateProfileString("机型信息", jx, "Error", d_info, MAX_DEVINFO_SIZE, JIXING);
		if (!strcmp(d_info, "Error"))
		{
			break;
		}
		// 解析配置文件中机型的字符串信息
		readDevInfoFromString(d_info, &allDev[k-1]);
		//GetPrivateProfileString("机型信息", jx, "请更新机型信息", ap, 100, JIXING);
		combobox1->AddString(allDev[k-1].product_name);
	}

	combobox1->SetCurSel(CON_CurSel);

	//***************************初始化界面中的产品列表:end*************************


	//***************************初始化界面中的测试结果统计:star*********************
	char num[10];

	sprintf(num, "%d", passtimes);
	CStatic* edit1=(CStatic*)GetDlgItem(IDC_PASSTIMES);
	edit1->SetWindowText(num);

	sprintf(num, "%d", failtimes);
	CStatic* edit2=(CStatic*)GetDlgItem(IDC_FAILTIMES);
	edit2->SetWindowText(num);

	sprintf(num, "%.2f%%", passpro);
	CStatic* edit3=(CStatic*)GetDlgItem(IDC_PASSPRO);
	edit3->SetWindowText(num);
	//***************************初始化界面中的测试结果统计:end**********************


	// 初始化界面中MAC显示里的列表
	m_RecordList.SetExtendedStyle(LVS_EX_FLATSB   //设置扫描记录表的扩展风格
		|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP
		|LVS_EX_ONECLICKACTIVATE
		|LVS_EX_GRIDLINES);
	m_RecordList.InsertColumn(0,"MAC",LVCFMT_LEFT,130,0);
    m_RecordList.InsertColumn(1,"测试结果",LVCFMT_LEFT,140,1);
    m_RecordList.InsertColumn(2,"吞吐量值",LVCFMT_LEFT,120,2);

	// 初始化界面中测试设置的默认IP
	dwAddress = inet_addr(CON_DUTIP);
	dwAddress = ntohl(dwAddress);
	m_DUTIP.SetAddress(dwAddress);

	// 初始化界面中测试设置的对端IP
	dwAddress = inet_addr(CON_remoteIP);
	dwAddress = ntohl(dwAddress);
	m_RemoteIP.SetAddress(dwAddress);

	// 初始化界面中测试设置的测试文件和吞吐量值
	m_TestFilename.SetWindowText(CON_testFilename);
	str.Format("%d", CON_passAvg_24g);
	m_PassAvg.SetWindowText(str);
	str.Format("%d", CON_passAvg_5g);
	m_PassAvg_5g.SetWindowText(str);

	// 隐藏测试结果图
	GetDlgItem(IDC_PASS_PIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_FAIL_PIC)->ShowWindow(FALSE);

	// 设置配置测试项是否可选
	if (0 == CON_EDITCONFIG)
	{
		GetDlgItem(IDC_CFORMAT)->EnableWindow(0);
		GetDlgItem(IDC_C3GMODEM)->EnableWindow(0);
		GetDlgItem(IDC_CPIN)->EnableWindow(0);
		GetDlgItem(IDC_CWAN)->EnableWindow(0);
		GetDlgItem(IDC_CRST)->EnableWindow(0);
		GetDlgItem(IDC_CVER)->EnableWindow(0);
	}

	// 如果启用控制WIFI按钮，则其他测试相关按钮不可用
	if (1 == WIFI_CONTROL)
	{
		GetDlgItem(IDC_BOPENWIFI)->ShowWindow(TRUE);
		GetDlgItem(IDC_BOPENWIFI)->EnableWindow(0);
		GetDlgItem(IDC_TESTTHROUGHPUT)->EnableWindow(0);
		GetDlgItem(IDC_FEEDBK)->EnableWindow(0);
		GetDlgItem(IDC_TYPESYN)->EnableWindow(0);
		GetDlgItem(IDC_RESET)->EnableWindow(0);
	}

	// 版本检查默认勾选
	((CButton*)GetDlgItem(IDC_CVER))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_IS_THROUGH_TEST))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_IS_AUDIO_TEST))->SetCheck(0);
}

/*******************************************************************************
  Function:    initialOpera
  Description: 通过查询数据库或者配置文件来判断需要操作的功能
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int CMyDlg::initialOpera(char *dev_name)
{
	CString sql;
	ADOConn m_AdoConn;
	_RecordsetPtr m_pRecordset;

	char buf[10];

	if (0 == CON_DBConnect)
	{
		return TRUE;
	}

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	// 通过查询数据库来获得吞吐量测试项
	// 联合查询DeviceModel表和ThroughputConfig表
	sql.Format("select * from DeviceModel,ThroughputConfig where t_model=d_id and \
	              d_name = '%s' ", dev_name);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			MessageBox("该机型版本不存在，请联系网管解决！","提示",MB_OK);
			m_AdoConn.ExitConnect();
			return FALSE;
		}
		else
		{
			product_test.test_ver = m_pRecordset->GetCollect("t_ver");
			product_test.test_mac = m_pRecordset->GetCollect("t_mac");
			product_test.test_imei = m_pRecordset->GetCollect("t_imei");
			product_test.test_meid = m_pRecordset->GetCollect("t_meid");
			product_test.test_pin = m_pRecordset->GetCollect("t_pin");
			product_test.test_rst = m_pRecordset->GetCollect("t_rst");
			product_test.test_wan = m_pRecordset->GetCollect("t_wan");
			product_test.test_3gmodem = m_pRecordset->GetCollect("t_3gmodem");
			product_test.test_format = m_pRecordset->GetCollect("t_format");

			strcpy(CON_SoftVer,(char*)(_bstr_t)m_pRecordset->GetCollect("d_softwarever"));
			strcpy(CON_HardVer,(char*)(_bstr_t)m_pRecordset->GetCollect("d_hardwarever"));
			strcpy(CON_DUTIP, (char*)(_bstr_t)m_pRecordset->GetCollect("d_ip"));
			strcpy(CON_DUTUSER, (char*)(_bstr_t)m_pRecordset->GetCollect("d_username"));
			strcpy(CON_DUTPASSWORD, (char*)(_bstr_t)m_pRecordset->GetCollect("d_password"));

			CON_DevType = m_pRecordset->GetCollect("d_serial");

			// 根据测试配置项来更新代表主界面左边选项的内容
			CON_FormatDisk = product_test.test_format;
			CON_3GModem = product_test.test_3gmodem;
			CON_PINcheck = product_test.test_pin;
			CON_WANTEST = product_test.test_wan;
			CON_Reset = product_test.test_rst;
			CON_CHECKVER = product_test.test_ver;

			CON_passAvg_24g = m_pRecordset->GetCollect("t_throughput");
			CON_passAvg_5g = m_pRecordset->GetCollect("t_throughput_5g");
			itoa(CON_passAvg_24g, buf, 10);
			m_PassAvg.SetWindowText(buf);
			itoa(CON_passAvg_5g, buf, 10);
			m_PassAvg_5g.SetWindowText(buf);

			dev_model_id = m_pRecordset->GetCollect("t_model");
        }
	}
	catch(...)
	{
		MessageBox("数据传输失败，请重新检查网络设置！","提示",MB_OK);
		m_AdoConn.ExitConnect();
		return FALSE;
	}


	// 通过查询数据库来获得写码工具配置项
	// 联合查询DeviceModel表和InfoWriteConfig表
	sql.Format("select * from DeviceModel,InfoWriteConfig where i_model=d_id and \
			   d_name = '%s' ", dev_name);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			MessageBox("该机型版本不存在，请联系网管解决！","提示",MB_OK);
			m_AdoConn.ExitConnect();
			return FALSE;
		}
		else
		{
			prodcut_info_write.has_sn = m_pRecordset->GetCollect("i_sn");
			prodcut_info_write.has_mac = m_pRecordset->GetCollect("i_mac");
			prodcut_info_write.has_meid = m_pRecordset->GetCollect("i_meid");
			prodcut_info_write.has_imei = m_pRecordset->GetCollect("i_imei");
			prodcut_info_write.has_pin = m_pRecordset->GetCollect("i_pin");
		}
	}
	catch(...)
	{
		MessageBox("数据传输失败，请重新检查网络设置！","提示",MB_OK);
		m_AdoConn.ExitConnect();
		return FALSE;
	}

    m_AdoConn.ExitConnect();

	if( CON_FormatDisk == 1 )
	{
		((CButton*)GetDlgItem(IDC_CFORMAT))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CFORMAT))->SetCheck(0);
	}

	if( CON_3GModem == 1 )
	{
		((CButton*)GetDlgItem(IDC_C3GMODEM))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_C3GMODEM))->SetCheck(0);
	}

	if ( CON_PINcheck == 1 )
	{
		((CButton*)GetDlgItem(IDC_CPIN))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CPIN))->SetCheck(0);
	}

	if( CON_WANTEST == 1 )
	{
		((CButton*)GetDlgItem(IDC_CWAN))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CWAN))->SetCheck(0);
	}

	if ( CON_Reset == 1 )
	{
		((CButton*)GetDlgItem(IDC_CRST))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CRST))->SetCheck(0);
	}

	if ( CON_CHECKVER == 1 )
	{
		((CButton*)GetDlgItem(IDC_CVER))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CVER))->SetCheck(0);
	}
	return TRUE;
}


/*******************************************************************************
  Function:    checkOpera
  Description: 通过检查复选框是否被选中来再次判断需要操作的功能
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
void CMyDlg::checkOpera()
{
	// 是否测试WAN口
	if( BST_CHECKED ==  ((CButton*)GetDlgItem(IDC_CWAN))->GetCheck())
	{
		CON_WANTEST = 1;
	} else
	{
		CON_WANTEST = 0;
	}

	// 是否检查PIN码
	if( BST_CHECKED ==  ((CButton*)GetDlgItem(IDC_CPIN))->GetCheck())
	{
		CON_PINcheck = 1;
	} else
	{
		CON_PINcheck = 0;
	}

	// 是否恢复出厂设置
	if( BST_CHECKED ==  ((CButton*)GetDlgItem(IDC_CRST))->GetCheck())
	{
		CON_Reset = 1;
	} else
	{
		CON_Reset = 0;
	}

	// 是否格式化用户盘
	if( BST_CHECKED ==  ((CButton*)GetDlgItem(IDC_CFORMAT))->GetCheck())
	{
		CON_FormatDisk = 1;
	} else
	{
		CON_FormatDisk = 0;
	}

	// 是否进行3G modem适配
	if( BST_CHECKED ==  ((CButton*)GetDlgItem(IDC_C3GMODEM))->GetCheck())
	{
		CON_3GModem = 1;
	} else
	{
		CON_3GModem = 0;
	}

	// 是否进行版本和MAC检查
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CVER))->GetCheck())
	{
		CON_CHECKVER = 1;
	} else
	{
		CON_CHECKVER = 0;
	}

	// 是否进行性能测试
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_IS_THROUGH_TEST))->GetCheck())
	{
		CON_THROUGH_TEST = 1;
	} else
	{
		CON_THROUGH_TEST = 0;
	}

	// 是否进行音频测试
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_IS_AUDIO_TEST))->GetCheck())
	{
		CON_AUDIO_TEST = 1;
	} else
	{
		CON_AUDIO_TEST = 0;
	}
}


/*******************************************************************************
  Function:    saveIniFile
  Description: 在退出程序关闭窗口前时执行的保存配置文件的操作
  Author:      liguoliang
  Date:        2013-12-17
*******************************************************************************/
void CMyDlg::saveIniFile()
{
	char i2a[10]={0};

	readDUTConfigure();
	readThroughputConfigure();

	WritePrivateProfileString("DUT参数", "默认IP", CON_DUTIP, CONFIGURE_FILENAME);
	WritePrivateProfileString("吞吐量测试配置", "测试文件名", CON_testFilename, CONFIGURE_FILENAME);
	WritePrivateProfileString("吞吐量测试配置", "对端IP", CON_remoteIP, CONFIGURE_FILENAME);

	itoa(CON_passAvg_24g,i2a,10);
	WritePrivateProfileString("吞吐量测试配置", "吞吐量_24g", i2a, CONFIGURE_FILENAME);

	itoa(CON_passAvg_5g,i2a,10);
	WritePrivateProfileString("吞吐量测试配置", "吞吐量_5g", i2a, CONFIGURE_FILENAME);

	itoa(passtimes,i2a,10);
	WritePrivateProfileString("测试统计", "测试合格次数", i2a, CONFIGURE_FILENAME);

	itoa(failtimes,i2a,10);
	WritePrivateProfileString("测试统计", "测试不合格次数", i2a, CONFIGURE_FILENAME);

	sprintf(i2a, "%.2f", passpro);
	WritePrivateProfileString("测试统计", "一次测试直通率", i2a, CONFIGURE_FILENAME);

}


/*******************************************************************************
  Function:    updateCurState
  Description: 更新主界面的软件运行状态信息
  Author:      liguoliang
  Date:        2013-12-20
*******************************************************************************/
void CMyDlg::updateCurState(CString str)
{
	strCurState += str;
	((CStatic*)GetDlgItem(IDC_CURSTATE))->SetWindowText(strCurState);
}


/*******************************************************************************
  Function:    updateResult
  Description: 更新主界面的测试过程结果
  Author:      liguoliang
  Date:        2013-12-20
*******************************************************************************/
void CMyDlg::updateResult(char *str, int color)
{
	strcpy(StrResult, str);
	drawResult(color, str);
}

/*******************************************************************************
  Function:    updatePassPic
  Description: 更改主界面PASS图片的可见性
  Author:      liguoliang
  Date:        2014-01-14
*******************************************************************************/
void CMyDlg::updatePassPic(int visible)
{
	if (1 == visible)
	{
		GetDlgItem(IDC_PASS_PIC)->ShowWindow(TRUE);
	} else
	{
		GetDlgItem(IDC_PASS_PIC)->ShowWindow(FALSE);
	}
}

/*******************************************************************************
  Function:    updateFailPic
  Description: 更改主界面FAIL图片的可见性
  Author:      liguoliang
  Date:        2014-01-14
*******************************************************************************/
void CMyDlg::updateFailPic(int visible)
{
	if (1 == visible)
	{
		GetDlgItem(IDC_FAIL_PIC)->ShowWindow(TRUE);
	} else
	{
		GetDlgItem(IDC_FAIL_PIC)->ShowWindow(FALSE);
	}
}

/*******************************************************************************
  Function:    changeTestButton
  Description: 更新主界面的测试过程结果
  Author:      liguoliang
  Date:        2013-12-20
*******************************************************************************/
void CMyDlg::changeTestButton(int begin_or_stop)
{
	runState.pingDUT = BEGIN_PING_DUT;
	if (BEGIN_TEST == begin_or_stop)
	{
		((CButton*)GetDlgItem(IDC_PING))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_TESTTHROUGHPUT))->SetWindowText("停止测试");
		((CButton*)GetDlgItem(IDC_TESTTHROUGHPUT))->EnableWindow(FALSE);
		((CButton *)this->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(FALSE);
	}
	else if (STOP_TEST == begin_or_stop)
	{
		((CButton*)GetDlgItem(IDC_PING))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_TESTTHROUGHPUT))->SetWindowText("开始测试");
		((CButton*)GetDlgItem(IDC_TESTTHROUGHPUT))->EnableWindow(TRUE);
	}
}

char *w2c(char *pcstr,const wchar_t *pwstr, size_t len)

{

	int nlength=wcslen(pwstr);

	//获取转换后的长度

	int nbytes = WideCharToMultiByte( 0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,     // wide character string to convert

		nlength,   // the number of wide characters in that string

		NULL,      // no output buffer given, we just want to know how long it needs to be

		0,

		NULL,      // no replacement character given

		NULL );    // we don't want to know if a character didn't make it through the translation

	// make sure the buffer is big enough for this, making it larger if necessary

	if(nbytes>len)   nbytes=len;

	// 通过以上得到的结果，转换unicode 字符为ascii 字符

	WideCharToMultiByte( 0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,   // wide character string to convert

		nlength,   // the number of wide characters in that string

		pcstr, // put the output ascii characters at the end of the buffer

		nbytes,                           // there is at least this much space there

		NULL,      // no replacement character given

		NULL );

	return pcstr ;

}

void c2w_acp(wchar_t *pwstr,size_t len,const char *str)

{

	if(str)

	{

		size_t nu = strlen(str);

		size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,NULL,0);

		if(n>=len)n=len-1;

		MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,pwstr,(int)n);

		pwstr[n]=0;

	}

}

void c2w_utf8(wchar_t *pwstr,size_t len,const char *str)

{

	if(str)

	{

		size_t nu = strlen(str);

		size_t n =(size_t)MultiByteToWideChar(/*CP_ACP*/CP_UTF8,0,(const char *)str,(int)nu,NULL,0);

		if(n>=len)n=len-1;

		MultiByteToWideChar(/*CP_ACP*/CP_UTF8,0,(const char *)str,(int)nu,pwstr,(int)n);

		pwstr[n]=0;

	}

}

/*******************************************************************************
  Function:    updateDevicInfo
  Description: 更新主界面的设备信息
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
void CMyDlg::updateDevicInfo()
{
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVSWV))->SetWindowText(productWebInfo.product_sw_ver);
	wchar_t hw_ver_unicode[100];
	if (strstr(productWebInfo.product_hw_ver, "暂无信息"))
	{
		((CStatic*)g_Dlg->GetDlgItem(IDC_DEVHWV))->SetWindowText(productWebInfo.product_hw_ver);
	} else
	{
		/* TR961 2500 移动版项目中统一在获取设备信息后进行了unicode的
		转码，故这里直接显示即可
		*/
		if (DEVICE_LTE_CMCC == CON_DevType)
		{
			((CStatic*)g_Dlg->GetDlgItem(IDC_DEVHWV))->SetWindowText(productWebInfo.product_hw_ver);
		}
		else
		{
			c2w_utf8(hw_ver_unicode,100, productWebInfo.product_hw_ver);
			::SetWindowTextW(GetDlgItem(IDC_DEVHWV)->GetSafeHwnd(), hw_ver_unicode);
		}
	}
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVIMEI))->SetWindowText(productWebInfo.product_imei);
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVMEID))->SetWindowText(productWebInfo.product_meid);
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVMAC))->SetWindowText(productWebInfo.product_mac);
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVWORKMODE))->SetWindowText(productWebInfo.product_work_mode);
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEV3GMODEM))->SetWindowText(productWebInfo.product_modem_status);
	((CStatic*)g_Dlg->GetDlgItem(IDC_DEVPIN))->SetWindowText(productWebInfo.product_pin);
}


/*******************************************************************************
  Function:    updateWiFiBandInfo
  Description: 更新主界面的WiFi Band信息
  Author:      Li Yuan
  Date:        2014-10-15
*******************************************************************************/
void CMyDlg::updateWiFiBandInfo()
{
//	((CStatic *)g_Dlg->GetDlgItem(IDC_PINLABEL))->SetWindowText("Wi-Fi 频段:");
	((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->EnableWindow(TRUE);
	((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->SetWindowText("正在获取...");

	bool get_band_ok = false;
	enum WiFiBand band = product->getWiFiBand();
	if (band == BAND_2_GHZ) {
		get_band_ok = true;
		((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->SetWindowText("2.4 GHz");
	} else if (band == BAND_5_GHZ) {
		get_band_ok = true;
		((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->SetWindowText("5 GHz");
	} else {
		((CStatic *)g_Dlg->GetDlgItem(IDC_DEVBAND))->SetWindowText("获取失败");
	}

	if (get_band_ok) {
		/* for msm8909, the band change is decided by config file */
		if ((CON_DevType == DEVICE_LTE_MSM8909 || CON_DevType == DEVICE_CARDV) && CON_DUAL_BAND_CONTROL == 0){
			((CButton *)g_Dlg->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(FALSE);
		}
		else{
			((CButton *)g_Dlg->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(TRUE);
		}
		((CButton *)g_Dlg->GetDlgItem(IDC_LTE_RESET))->EnableWindow(TRUE);
	} else {
		((CButton *)g_Dlg->GetDlgItem(IDC_CHANGE_BAND))->EnableWindow(FALSE);
		((CButton *)g_Dlg->GetDlgItem(IDC_LTE_RESET))->EnableWindow(FALSE);
	}
}
