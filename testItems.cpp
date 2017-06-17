/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   testItems.cpp

  Description: 定义测试项实现的函数，将原来在同一个文件中定义的函数分离出来

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-05, liguoliang      create file.
*******************************************************************************/

#include "StdAfx.h"
#include "testItems.h"
#include "ADOConn.h"
#include "WirelessTestDlg.h"
#include "configure.h"
#include "ThroughputTest.h"
#include "iphlpapi.h"
#include "icmp.h"
#include "fmifs.h"
#include "device.h"
#include "PassDlg.h"
#include "WSDataTransCpp.h"
#include <string.h>

extern CMyDlg * g_Dlg;
extern Device * product;
extern CWSDataTransCpp * mic;
// 路由配置 route ADD 172.0.0.0 MASK 255.0.0.0 172.28.130.1 METRIC 1 -p
extern char * devid;
static struct VersionInfos _version_infos;

CString throughput_msg_str;   // 显示在主界面的吞吐量测试值结果
CString throughput_value_str; // 显示在主界面的吞吐量数值

CString sql;                  // 数据库操作sql语句
ADOConn m_AdoConn;            // 数据库操作类对象

CTime curTime;                // 针对产品单次测试的时间

DWORD WINAPI timeOutThread(LPVOID pParam)
{
	int overtime_allowance = 10;
	time_t start,end;
	start =time(NULL);

	while(1)
	{
		Sleep(1000);
		end =time(NULL);

		if (runState.iperfStatus == IPERF_STOP)
		{
			break;
		}

		if (difftime(end,start) > CON_TEST_TIME + overtime_allowance)
		{
			/* when time out, kill iperf */
			if (CON_DevType == DEVICE_CARDV)
			{
				system("TASKKILL /F /IM iperf.exe /T");
			}
			break;
		}

	}
	return TRUE;
}


DWORD WINAPI audioResult(LPVOID pParam)
{
	FILE *fp;
	char buf[256] = {};
	char *Success = "pass";
	char *Fail = "fail";
	char *Error = "error";
	int max_try_times = 20;
	int try_time = 0;
	CString str;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);

	while (CLOSE_PING_DUT != runState.pingDUT && try_time < max_try_times)
	{
		try_time++;
		fp = fopen(".\\dist-audio\\cache\\result.txt", "r");
		if (fp)
	    {
			if (fgets(buf, sizeof(buf), fp))
			{
				if (strstr(buf, Success))
				{
					g_Dlg->updateCurState(">>音频测试通过");
					g_Dlg->updateResult("音频测试通过", RGB(0,255,0));
					CON_AUDIO_RESULT = 1;
					((CButton *)g_Dlg->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(TRUE);
				}

				if (strstr(buf, Fail))
				{
					g_Dlg->updateCurState(">>音频测试不通过");
					g_Dlg->updateResult("音频测试不通过", RGB(255,0,0));
					CON_AUDIO_RESULT = 0;
					((CButton *)g_Dlg->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(TRUE);
				}

				if (strstr(buf, Error))
				{
					g_Dlg->updateCurState(">>音频测试错误");
					g_Dlg->updateResult("音频测试错误", RGB(255,0,0));
					CON_AUDIO_RESULT = 0;
					((CButton *)g_Dlg->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(FALSE);
				}

			}
			fclose(fp);
	    }

		if (CON_AUDIO_RESULT != 2)
		{
			break;
		}

		if (runState.stop == TRUE)
		{
			str.Format("已停止运行");
			m_Avg->SetWindowText(str);
			system("TASKKILL /F /IM main.exe /T");
			break;
		}

		Sleep(1000);
	}

	if (CON_AUDIO_RESULT == 2)
	{
		g_Dlg->updateCurState(">>音频测试失败");
		g_Dlg->updateResult("音频测试失败", RGB(255,0,0));
	}

	return TRUE;
}


DWORD WINAPI iperfPCThread(LPVOID pParam)
{
	char  starttime[65];
	char endtime[65];
	CString str;
	CTime tm;

	double testValue = 0.0;
	throughput_msg_str = "";
	char test_in_pc[64]={0};
	int throughput_threshold= 0;

	//获取系统时间
	//CTime tm;
	tm=CTime::GetCurrentTime();
	str=tm.Format("%Y/%m/%d/ %X");
	sprintf(starttime, "%s", str);

	if (IPERF_TX_TEST == CON_TEST_TYPE)
	{
		sprintf(test_in_pc, CON_iperf_tx_pc);
	}
	else if (IPERF_RX_TEST == CON_TEST_TYPE)
	{
		sprintf(test_in_pc, CON_iperf_rx_pc, CON_DUTIP, CON_TEST_TIME);
	}

	while (CLOSE_PING_DUT != runState.pingDUT && IPERF_WORK_ERR != runState.iperfStatus)
	{
		//WaitForSingleObject(hMutex, INFINITE);
		if (IPERF_START_PC == runState.iperfStatus || IPERF_SWITCH_PC == runState.iperfStatus)
		{
			g_Dlg->updateCurState(">>开启PC端iperf");
			testValue = product->startIperfPC(test_in_pc);
		}
		else if (IPERF_STOP == runState.iperfStatus)
		{
			break;
		}
		Sleep(1000);
		//ReleaseMutex(hMutex);
	}

	throughput_msg_str.Format("平均吞吐量为：%.3f Mbps", testValue);
	throughput_value_str.Format("%.3f",testValue);
	SetDlgItemText(g_Dlg->hWnd, IDC_AVG, throughput_msg_str);

	// 保存测试结果到log中
	saveFileLog(throughput_value_str, curTime);

	enum WiFiBand band = product->getWiFiBand();
	if (band == BAND_2_GHZ) {
		throughput_threshold = CON_passAvg_24g;
	} else if (band == BAND_5_GHZ) {
		throughput_threshold = CON_passAvg_5g;
	} else {
		g_Dlg->updateCurState(">>BAND获取失败");
		g_Dlg->updateResult(" BAND获取失败", RGB(0,0,0));
		return TRUE;
	}

	// 如果测试结果小于预设速率值
	if (testValue < throughput_threshold)
	{
		throughputNoPass(throughput_value_str);
	}
    else
    {
    	throughputPass(throughput_value_str);
    }

	tm=CTime::GetCurrentTime();
	str=tm.Format("%Y/%m/%d/ %X");
	sprintf(endtime, "%s", str);

	if (CON_MES_ENABLE)
	{
		if (uploadData(throughput_value_str, starttime, endtime, throughput_threshold) < 0)
		{
			CON_FORP = 0;
		}
	}

	insertTestRecord(testValue, throughput_threshold);
	g_Dlg->m_RecordList.DeleteItem(20);

	return TRUE;

}

DWORD WINAPI iperfDUTThread(LPVOID pParam)
{
	char test_in_dut[64]={0};
	int start_iperf_success = 1;

	if (IPERF_TX_TEST == CON_TEST_TYPE)
	{
		sprintf(test_in_dut, CON_iperf_tx_dut, CON_remoteIP, CON_TEST_TIME);
	}
	else if (IPERF_RX_TEST == CON_TEST_TYPE)
	{
		sprintf(test_in_dut, CON_iperf_rx_dut);
	}

	//char test_in_dut[]="-s";
	while (CLOSE_PING_DUT != runState.pingDUT)
	{
		//WaitForSingleObject(hMutex_iperf, INFINITE);
		if (IPERF_START_DUT == runState.iperfStatus || IPERF_SWITCH_DUT == runState.iperfStatus)
		{
            WaitForSingleObject(hMutex_iperf, INFINITE);
			g_Dlg->updateCurState(">>开启DUT端iperf");
			if (product->startIperfDUT(test_in_dut) != 0)
			{
				// TODO:
				g_Dlg->updateCurState(">>开启DUT端iperf失败");
				runState.iperfStatus = IPERF_WORK_ERR;
				start_iperf_success = 0;
			}
            ReleaseMutex(hMutex_iperf);
		}
		else if (IPERF_STOP == runState.iperfStatus)
		{
			break;
		}

		if (start_iperf_success == 0)
		{
			return TRUE;
		}
		//ReleaseMutex(hMutex_iperf);
		Sleep(1000);
	}
    return TRUE;
}



/*********************************************************
*函 数 名：   throughputTest
*摘    要：   测试吞吐量，若第一次不合格则再测一次，
*             在该函数中，主要是调用beginThroughputTest()
*作    者：   邓国伟
*创建日期：   2007-10-15
*历史版本：   2013-12-10 liguoliang
*********************************************************/
DWORD WINAPI throughputTest(LPVOID pParam)
{

	runState.pingDUT = PAUSE_PING_DUT;
	sql = "";

	if (DEVICE_LTE_CMCC != CON_DevType)
	{
		g_Dlg->strCurState = "";
	}

	g_Dlg->hWnd = g_Dlg->GetSafeHwnd();
	curTime=CTime::GetCurrentTime();

	// 隐藏PASS图，清空MAC显示内容
	g_Dlg->updatePassPic(0);
	g_Dlg->updateFailPic(0);
	g_Dlg->SetDlgItemText(IDC_DUTMAC,"");

	closeShield();//关闭屏蔽箱

	Sleep(100);
	avgThroughput[0]= 0;
	if (0 == strcmp(StrResult, "         停止"))
	{
		g_Dlg->updateResult("", RGB(0,0,0));
	}

    runState.begin = FALSE;

	// 清空ARP表、连接DUT和对方主机
	if(FALSE == connectDev())
	{
		goto test_finish;
	}

	////////////////////////////////////////////////////////////////////////////
	//吞吐量测试前先进行一些设备信息数据的检查，对检查存在问题的设备会提醒是否继续进行吞吐量测试
	////////////////////////////////////////////////////////////////////////////

	// 获取设备MAC并从数据库中得到该设备的其他信息
	if (!getDBProductInfo())
	{
		goto test_finish;
	}

	// 根据MAC查询是否已经存在对应的测试记录
	if (TRUE == checkExistDBLog())
	{
		goto test_finish;
	}

	// 判断页面显示的软硬件版本、MAC、IMEI/MEID等信息等是否匹配
	if(!testVersion())
	{
		goto test_finish;
	}

	// WAN口测试，为非必须测试项
	if(!testWAN())
	{
		goto test_finish;
	}

	// PIN码测试，为非必须测试项
	if(!testPIN())
	{
		goto test_finish;
	}

	// 格式化用户盘，为非必须测试项
	if(!formatDUT())
	{
		goto test_finish;
	}

	// 3G modem适配，为非必须测试项
	if(!test3GModem())
	{
		goto test_finish;
	}

	// MIC 检查，为非必须项
	if (!testMIC())
	{
		goto test_finish;
	}

	if (!testType())
	{
		goto test_finish;
	}

	if (CON_THROUGH_TEST == 1)
	{
		if (CON_TEST_TYPE == IPERF_TX_TEST
			|| CON_TEST_TYPE == IPERF_RX_TEST)
		{
			iperfTest();
		}
		else if (CON_TEST_TYPE == CHARIOT_TEST)
		{
			chariotTest();
		}
	}

	/* 只有在吞吐量测试通过或者不测吞吐量时才能测试音频 */
	if (CON_FORP == 1 || CON_THROUGH_TEST == 0)
	{
		if (CON_AUDIO_TEST == 1)
		{
			audioTest();
		}
	}

test_finish:
	openShield();//打开屏蔽箱

	if ((1 != CON_FORP && 1 == CON_THROUGH_TEST)
		|| (1 != CON_AUDIO_RESULT && 1 == CON_AUDIO_TEST))
	{
		failtimes++;  //记录不合格次数
		ft.Format("%d",failtimes);
		SetDlgItemText(g_Dlg->hWnd, IDC_FAILTIMES, ft);

		g_Dlg->updateFailPic(1);
		g_Dlg->updatePassPic(0);
	}
	else
	{
		//合格
		passtimes++;//记录合格次数
		pt.Format("%d",passtimes);
		SetDlgItemText(g_Dlg->hWnd, IDC_PASSTIMES, pt);

		g_Dlg->updateFailPic(0);
		g_Dlg->updatePassPic(1);
	}

	float buffer = passtimes;	//强制转换为float型，从而进行下面的除法运算
	passpro = buffer/(buffer+failtimes) *100;
	pp.Format("%.2f%s",passpro,"%");
	SetDlgItemText(g_Dlg->hWnd, IDC_PASSPRO, pp);

	g_Dlg->changeTestButton(STOP_TEST);
	DUTState = DUT_STATE_TESTED;

	return TRUE;
}

/*********************************************************
*函 数 名：   insertTestRecord
*摘    要：   将测试记录添加到有recordCount个元素的循环数组中
*作    者：   邓国伟
*创建日期：   2007-10-22
*********************************************************/
void insertTestRecord(double testValue, int throughput_threshold)
{
	char result[10];
	SYSTEMTIME localtime;
	GetLocalTime(&localtime);

	if (testValue >= throughput_threshold)
	{
		strcpy(result, " 合格 ");
	}
	else
	{
		strcpy(result, "不合格");
	}

	if (counts < recordCount)
	{
		testRecord[counts].Format(" %02d:%02d:%02d   %s     %s       %.3f",
			localtime.wHour, localtime.wMinute,localtime.wSecond,
			DUTMAC, result, testValue);
		counts++;
	}
	else
	{
		testRecord[begRecordPos].Format(" %02d:%02d:%02d   %s     %s       %.3f",
			localtime.wHour, localtime.wMinute,localtime.wSecond,
			DUTMAC, result,testValue);
		begRecordPos = (begRecordPos + 1) % recordCount;
	}
}


bool pingWAN(char IPaddr[16])
{
	FILE *fp;
	char tmp[256];
	CString strPing;

	// [liguoliang start] 修改ping的方式是通过调用dll中的函数来完成
	if (icmpDll != NULL)
	{
		if (myPing(IPaddr) == TRUE)
		{
			return TRUE;
		}
	}
	return FALSE;
	// [liguoliang end]

}

/*******************************************************************************
  Function:    FormatExCallback
  Description: 调用格式化用户盘的函数的回调函数
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
BOOLEAN __stdcall FormatExCallback( CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument )
{
	PBOOLEAN status;
	switch( Command )
	{
	case DONE:
		status = (PBOOLEAN) Argument;
		if( *status == FALSE ) {
			// 显示在界面上
		} else {
			// 显示在界面上
		}
		break;
	}
	return TRUE;
}

// 将窄字符串转换成宽字符串
static wstring widen( const string& str )
{
	wostringstream wstm ;
	const ctype<wchar_t>& ctfacet =
		use_facet< ctype<wchar_t> >( wstm.getloc() ) ;
	for( size_t i=0 ; i<str.size() ; ++i )
		wstm << ctfacet.widen( str[i] ) ;
	return wstm.str() ;
}

/*******************************************************************************
  Function:    formatDisk
  Description: 针对7些列有存储盘的的机型，格式化存储盘
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int formatDisk()
{
	char diskPath[5] = { 0 };

	DWORD driveType;
	DWORD allDisk = GetLogicalDrives();

	if (allDisk!=0)
	{
		for (int i=0;i<15;i++) //假定最多有15个磁盘
		{
			if ((allDisk & 1)==1)
			{
				sprintf( diskPath, "%c", 'a'+i );
				strcat( diskPath, ":\\" );

				if ( GetDriveType(diskPath)==DRIVE_REMOVABLE )
				{
					char lpVolumeNameBuffer[32] = {0};

					if( GetVolumeInformation(diskPath,lpVolumeNameBuffer,32,0,0,0,0,0) ) //判断驱动是否准备就绪
					{
						// 只有当用户盘标签为TP-LINK的时候才格式化，以免把其他U盘格式化了
						if (strcmp(lpVolumeNameBuffer, "TP-LINK") == 0)
						{
							// 调用fmifs.dll的函数执行格式化的操作
							if (fmisfDll != NULL)
							{
								FormatEx((PWCHAR)widen(diskPath).c_str(), FMIFS_REMOVEABLEDISK, L"FAT32", L"TP-LINK", TRUE,512, FormatExCallback );
							}
							return TRUE;
						}

					}
				}
			}
			allDisk = allDisk>>1;
		}
	}

	return FALSE;
}

/*******************************************************************************
  Function:    AddError
  Description: 分离CMyDlg中的AddError函数成单独的函数
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
void AddError(int er)
{
	//暂时不保存配置测试结果记录
	return;

	CString sqlset;
	CTime time1;
	time1=CTime::GetCurrentTime();

	if (CON_DBConnect == 0)
	{
		return;
	}

	ADOConn m_AdoConn1;
	sqlset.Format("select * from ThroughputLog where tl_pid = %d ", product_id);
	_RecordsetPtr m_pRecordset;
	m_pRecordset=m_AdoConn1.GetRecordSet((_bstr_t)sqlset);

	try
	{
		// [liguoliang start] 删除重复部分

		//如果不存在MAC，直接插入
		//if(m_AdoConn1.m_pRecordset->adoEOF!=0)
		{
			m_pRecordset->AddNew();
		}

		m_pRecordset->PutCollect("tl_pid",_variant_t((long)product_id));
		m_pRecordset->PutCollect("tl_result",(short)0);
		m_pRecordset->PutCollect("tl_value",(_bstr_t)"0");
		m_pRecordset->PutCollect("tl_userno",(_bstr_t)nb);
		m_pRecordset->PutCollect("tl_time",(_bstr_t)time1.Format("%Y-%m-%d %H:%M:%S"));

		if ( er == PASS_NOERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"OK");
			g_Dlg->m_RecordList.SetItemText(0,1,"合格");
			CON_FORP = 1;
		}
		else if ( er == SOFTWARE_UNMATCH )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"软件版本错误");
			g_Dlg->m_RecordList.SetItemText(0,1,"软件版本错误");
		}
		else if ( er == HARDWARE_UNMATCH )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"硬件版本错误");
			g_Dlg->m_RecordList.SetItemText(0,1,"硬件版本错误");
		}
		else if ( er == PIN_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"PIN码错误");
			g_Dlg->m_RecordList.SetItemText(0,1,"PIN码错误");
		}
		else if ( er == MAC_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"MAC错误");
			g_Dlg->m_RecordList.SetItemText(0,1,"MAC错误");
		}
		else if ( er == WAN_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"WAN口ping不通");
			g_Dlg->m_RecordList.SetItemText(0,1,"WAN口ping不通");
		}

		m_pRecordset->Update();
		m_AdoConn1.ExitConnect();
		// [liguoliang end]

	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");
	}
}

/*******************************************************************************
  Function:    getMACInfo
  Description: 通过发送ARP包获取MAC地址
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
int getMACInfo(char *ip, char *resultBuf)
{
	int ret = 0;

	if (NULL == resultBuf)
	{
		return ret;
	}

	int nRemoteAddr = inet_addr(ip);

	//发送ARP查询包获得远程MAC地址
	ULONG macAddLen = 6;
	unsigned char macBuf[6];
	ret=SendARP(nRemoteAddr, (unsigned long)NULL,(PULONG)macBuf, &macAddLen);
	if ( ret == NO_ERROR )
	{
		ret = 1;

		sprintf(resultBuf, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", macBuf[0],macBuf[1],
					macBuf[2],macBuf[3],macBuf[4],macBuf[5] );

	}
	else
	{
		ret = 0;
	}

	return ret;
}


/*******************************************************************************
  Function:    connectDev
  Description: 连接DUT和对方主机
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN connectDev()
{
	CString pingcmd;
	WinExec("arp -d", SW_HIDE);  // 清空ARP表
	//pingcmd.Format("ping -n 1 -l 1000 %s",CON_DUTIP);
	//WinExec(pingcmd,SW_HIDE);

	//if (FALSE == pingRequest(CON_DUTIP))
	if (FALSE == DUT_CONNECT)
	{
		g_Dlg->updateCurState(">>DUT未启动");
		g_Dlg->updateResult("         停止", RGB(0,0,0));
		runState.stop = TRUE;
		return FALSE;
	}

	if (FALSE == MIC_CONNECT)
	{
		g_Dlg->updateCurState(">>MIC服务器未连接");  // 未连接下可以继续测试
	}

	g_Dlg->updateCurState(">>DUT已启动");
	g_Dlg->updateResult("  测试中", RGB(0,0,0));

	//等待和对方主机ping通
	g_Dlg->updateCurState(">>等待和对方建立连接");

	if (TRUE != pingRequest(CON_remoteIP))
	{
		g_Dlg->updateCurState(">>未建立连接>>停止");
		g_Dlg->updateResult("         停止", RGB(0,0,0));

		return FALSE;
	}
	else
	{
		g_Dlg->updateCurState(">>已建立连接");
	}

	return TRUE;
}


/*******************************************************************************
  Function:    saveFileLog
  Description: 保存吞吐量测试结果到log文件中
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN saveFileLog(CString avg, CTime time)
{
	CString filename;
	filename.Format(".\\log\\%s.log",time.Format("%y%m%d"));
	FILE* fp;                 //将MAC地址记录到log文件中
	fp=fopen(filename,"a");

	fprintf(fp,"%s %s 平均吞吐量为：%s Mbps 测试员:%s\n",time.Format("%H时%M分"),DUTMAC,avg,nb);
	fclose(fp);

	g_Dlg->m_RecordList.InsertItem(0,"",0);
	g_Dlg->m_RecordList.SetItemText(0,0,DUTMAC);
	g_Dlg->m_RecordList.SetItemText(0,2,avg);

	return TRUE;
}


/*******************************************************************************
  Function:    testWAN
  Description: 测试WAN口
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testWAN()
{
	if( CON_WANTEST == 1 ) // WAN口检测
	{
		g_Dlg->updateResult("测试WAN口", RGB(0,0,0));
		DWORD k=pingWAN(CON_WANIP);
		if (k != TRUE)
		{
			error_no = WAN_ERROR;
			AddError(error_no);

			g_Dlg->updateCurState(">>WAN口ping不通>>停止");
			g_Dlg->updateResult("WAN口测试失败", RGB(255,0,0));

			return FALSE;
		} else
		{
			g_Dlg->updateCurState(">>WAN口测试通过");
		}
	}
	return TRUE;
}


/*******************************************************************************
  Function:    resetDUT
  Description: 软件复位
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int resetDUT()
{
	CString str;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);

	if ( CON_Reset == 1 ) //复位
	{
		g_Dlg->updateResult("正在复位软件", RGB(0,0,0));
		CString msg;
		msg.Format(">>发送复位数据包，设置等待时间为%d", CON_RESET_TIME/1000);
		g_Dlg->updateCurState(msg);

		int timeOut = product->reset();
		if(0 == timeOut)
		{
			AfxMessageBox("软件复位失败，请尝试登录管理页面手动进行恢复出厂设置");

			g_Dlg->updateResult("复位失败", RGB(255,0,0));
			g_Dlg->updateCurState(">>复位失败");

			return FALSE;
		} else
		{
			int time_second = timeOut/1000;
			for (int i = 0; i < time_second; i++)
			{
				str.Format("正在复位：已过 %d 秒", i+1);
				m_Avg->SetWindowText(str);
				if(FALSE == myPing(CON_DUTIP))
				{
					Sleep(3000);
					break;
				}
				Sleep(1000);
			}
		}

		g_Dlg->updateResult("   复位完成", RGB(0,0,0));
		g_Dlg->updateCurState(">>复位成功");
	}

	return TRUE;
}

extern void c2w_utf8(wchar_t *pwstr,size_t len,const char *str);
extern void c2w_acp(wchar_t *pwstr,size_t len,const char *str);

/*******************************************************************************
  Function:    testVersion
  Description: 检查软硬件版本和网页显示的MAC地址是否匹配等工作
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testVersion()
{
	if (0 == CON_DBConnect && DEVICE_CARDV != CON_DevType)
	{
		return TRUE;
	}

	if ( CON_CHECKVER == 1) // 检查版本信息和MAC地址
	{
		g_Dlg->updateResult("信息匹配检查", RGB(0,0,0));

		//核对软件版本和硬件版本
		g_Dlg->updateCurState(">>设备信息匹配测试");

		// 软件版本检查
		if ( 0 != strcmp(productDBInfo.product_sw_ver, "N/A") && DEVICE_CARDV != CON_DevType)
		{
			if (NULL == strstr(productDBInfo.product_sw_ver, productWebInfo.product_sw_ver))
			{
				g_Dlg->updateResult("软件版本错误", RGB(255,0,0));
				g_Dlg->updateCurState(">>软件版本错误");

				AfxMessageBox("软件版本错误");
				return FALSE;
			}
		}

		if (0 != strcmp(CON_CHECK_SOFTWARE, "N/A"))
		{
			if (NULL == strstr(CON_CHECK_SOFTWARE, productWebInfo.product_sw_ver))
			{
				g_Dlg->updateResult("软件版本错误", RGB(255,0,0));
				g_Dlg->updateCurState(">>软件版本错误");

				AfxMessageBox("软件版本错误");
				return FALSE;
			}
		}

		// 硬件版本检查
		if ( 0!= strcmp(productDBInfo.product_hw_ver, "N/A") && DEVICE_CARDV != CON_DevType)
		{
			/* TR961 2500 移动版项目中统一在获取设备信息后进行了unicode的
			转码，故这里无需在做处理
			*/
			if (DEVICE_LTE_CMCC == CON_DevType)
			{
				if (NULL == strstr(productDBInfo.product_hw_ver, productWebInfo.product_hw_ver))
				{
					g_Dlg->updateResult("硬件版本错误", RGB(255,0,0));
					g_Dlg->updateCurState(">>硬件版本错误");

					AfxMessageBox("硬件版本错误");
					return FALSE;
				}
			}
			else
			{
				// TODO:比较wchar和char的中文字符，未解决
				wchar_t webinfo_hw_ver_unicode[100];
				wchar_t db_hw_ver_unicode[100];
				c2w_utf8(webinfo_hw_ver_unicode,100, productWebInfo.product_hw_ver);
				c2w_acp(db_hw_ver_unicode,100, productDBInfo.product_hw_ver);

				if (NULL == StrStrIW(webinfo_hw_ver_unicode, db_hw_ver_unicode))
				//if (NULL == strstr(productDBInfo.product_hw_ver, productWebInfo.product_hw_ver))
				{
					g_Dlg->updateResult("硬件版本错误", RGB(255,0,0));
					g_Dlg->updateCurState(">>硬件版本错误");

					AfxMessageBox("硬件版本错误");
					return FALSE;
				}
			}
		}

		if (0 != strcmp(CON_CHECK_HARDWARE, "N/A"))
		{
			if (NULL == strstr(CON_CHECK_HARDWARE, productWebInfo.product_hw_ver))
			{
				g_Dlg->updateResult("硬件版本错误", RGB(255,0,0));
				g_Dlg->updateCurState(">>硬件版本错误");

				AfxMessageBox("硬件版本错误");
				return FALSE;
			}
		}

		// MAC地址检查

		if(((0 != product_test.test_mac) || DEVICE_CARDV == CON_DevType)  && (0 != mac_compare((LPSTR)(LPCTSTR)DUTMAC, productWebInfo.product_mac)))
		{
			g_Dlg->updateResult("网页MAC错误",RGB(255,0,0));
			g_Dlg->updateCurState(">>网页MAC地址错误");

			AfxMessageBox("网页MAC错误");
			return FALSE;
		}

		// IMEI检查
		/* TR961 2500 移动版项目中规定写入数据库的不合法IMEI
		都用16位表示
		*/
		if (DEVICE_LTE_CMCC == CON_DevType)
		{
			if((0 != product_test.test_imei) && (15 == strlen(productDBInfo.product_imei)))
			{
				if (0 != strcmp(productDBInfo.product_imei, productWebInfo.product_imei))
				{
					g_Dlg->updateResult("IMEI错误", RGB(255,0,0));
					g_Dlg->updateCurState(">>IMEI错误");

					AfxMessageBox("IMEI错误");
					return FALSE;
				}
			}
		}
		else
		{
			if((0 != product_test.test_imei) && (0!= strcmp(productDBInfo.product_imei, "N/A")))
			{
				if (0 != strcmp(productDBInfo.product_imei, productWebInfo.product_imei))
				{
					g_Dlg->updateResult("IMEI错误", RGB(255,0,0));
					g_Dlg->updateCurState(">>IMEI错误");

					AfxMessageBox("IMEI错误");
					return FALSE;
				}
			}
		}

		// MEID检查
		if ((0 != product_test.test_meid) && (0!= strcmp(productDBInfo.product_meid, "N/A")) && DEVICE_CARDV != CON_DevType)
		{
			if (0 != strcmp(productDBInfo.product_meid, productWebInfo.product_meid))
			{
				g_Dlg->updateResult("MEID错误", RGB(255,0,0));
				g_Dlg->updateCurState(">>MEID错误");

				AfxMessageBox("MEID错误");
				return FALSE;
			}
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    testPIN
  Description: 检查PIN码
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int testPIN()
{
	if (0 == CON_DBConnect)
	{
		return TRUE;
	}

	if ( CON_PINcheck == 1 )  //检查并保存PIN码
	{
		g_Dlg->updateResult("检查PIN码", RGB(0,0,0));
		g_Dlg->updateCurState(">>检查PIN码");

		// PIN码检查
		if ( 0 != strcmp(productDBInfo.product_pin, "N/A"))
		{
			if (0 != strcmp(productDBInfo.product_pin, productWebInfo.product_pin))
			{
				g_Dlg->updateResult("PIN码错误", RGB(255,0,0));
				g_Dlg->updateCurState(">>PIN码错误");

				AfxMessageBox("PIN码有问题");
				return FALSE;
			}
		}

	} // 结束PIN码测试

	return TRUE;
}


/*******************************************************************************
  Function:    throughputPass
  Description: 记录吞吐量合格
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputPass(CString throughput_value)
{
	//将测试结果传入数据库中
	error_no = PASS_NOERROR;  //错误代码

	g_Dlg->updateResult("吞吐量测试通过", RGB(0,255,0));
	g_Dlg->updateCurState(">>吞吐量测试通过");

	g_Dlg->m_RecordList.SetItemText(0,1,"合格");
	CON_FORP = 1;

	if (CON_DBConnect == 0)
	{
		return TRUE;
	}

	// 频段信息
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	g_Dlg->updateCurState(">>已连接数据库>>正在传入数据");

	_RecordsetPtr m_pRecordset;

	//将原来的每个MAC都插入改为先检索，如无相同MAC再插入
	sql.Format("select * from ThroughputLog where tl_pid = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// 不存在MAC，直接插入
		//if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			m_pRecordset->AddNew();
			g_Dlg->updateCurState(">>传入数据完毕(插入新MAC)");
		}
		//else // 如果存在MAC，直接更改
		//{
		//	strCurState += ">>传入数据完毕(MAC已存在)";
		//}

		m_pRecordset->PutCollect("tl_pid",(_variant_t)((long)product_id));
		m_pRecordset->PutCollect("tl_result",(_variant_t)1);
		m_pRecordset->PutCollect("tl_value",(_bstr_t)throughput_value);
		m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"OK");
		m_pRecordset->PutCollect("tl_userno",(_bstr_t)nb);
		m_pRecordset->PutCollect("tl_time",(_bstr_t)curTime.Format("%Y-%m-%d %H:%M:%S"));
		m_pRecordset->PutCollect("tl_band", (_variant_t)band_rec);
		m_pRecordset->Update();

		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");

		g_Dlg->updateCurState(">>传入数据失败");
		return FALSE;
	}

	changeDBProductStatus(1);
	return TRUE;
}


/*******************************************************************************
  Function:    throughputNoPass
  Description: 记录吞吐量不合格
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputNoPass(CString throughput_value)
{
	g_Dlg->m_RecordList.SetItemText(0,1,"速率低");

	g_Dlg->updateCurState(">>不合格");
	g_Dlg->updateResult("速率低", RGB(255,0,0));

	CON_FORP = 0;

	if (CON_DBConnect == 0)
	{
		return TRUE;
	}

	// 频段信息
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	g_Dlg->updateCurState(">>已连接数据库>>正在传入数据");

	CString sql;
	_RecordsetPtr m_pRecordset;

	//将原来的每个MAC都插入改为先检索，如无相同MAC再插入
	sql.Format("select * from ThroughputLog where tl_pid = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		//如果不存在MAC，直接插入
		//if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			m_pRecordset->AddNew();
			g_Dlg->updateCurState(">>传入数据完毕(插入新MAC)");
		}
		/*else
		{
		strCurState += ">>传入数据完毕(MAC已存在)";
		}*/

		m_pRecordset->PutCollect("tl_pid",(_variant_t)((long)product_id));
		m_pRecordset->PutCollect("tl_result",(_variant_t)0);
		m_pRecordset->PutCollect("tl_value",(_bstr_t)throughput_value);
		m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"OK");
		m_pRecordset->PutCollect("tl_userno",(_bstr_t)nb);
		m_pRecordset->PutCollect("tl_time",(_bstr_t)curTime.Format("%Y-%m-%d %H:%M:%S"));
		m_pRecordset->PutCollect("tl_band", (_variant_t)band_rec);
		m_pRecordset->Update();

		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");

		g_Dlg->updateCurState(">>传入数据失败");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	return TRUE;
}


/*******************************************************************************
  Function:    formatDUT
  Description: 格式化用户盘
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int formatDUT()
{
	if (CON_FormatDisk == 1) // 格式化用户盘
	{
		g_Dlg->updateResult("格式化用户盘",RGB(0,0,0));

		if(!formatDisk())
		{
			g_Dlg->updateCurState(">>用户盘格式化失败>>停止");
			g_Dlg->updateResult(" 格式化U盘失败", RGB(255,0,0));

			AfxMessageBox("格式化失败，不存在'TP-LINK'标签的用户盘!");
		}
		else
		{
			g_Dlg->updateCurState(">>格式化用户盘成功");
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    test3GModem
  Description: 检查3G Modem是否适配
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int test3GModem()
{
	if (CON_3GModem == 1) // 3G modem识别
	{
		g_Dlg->updateResult("检查3G MODEM", RGB(0,0,0));

		if (0 == strcmp(productWebInfo.product_modem_status, "N/A"))
		{
			g_Dlg->updateCurState(">>无法识别3G modem");
			AfxMessageBox("无法识别3G Modem!");

			return FALSE;
		} else if (0 == strcmp(productWebInfo.product_modem_status, "Ready"))
		{
			g_Dlg->updateCurState(">>3G modem已准备");
		} else if (0 == strcmp(productWebInfo.product_modem_status, "Not Ready"))
		{
			g_Dlg->updateCurState(">>3G modem未准备");
		} else
		{
			g_Dlg->updateCurState(">>3G modem状态未知");
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    testMIC
  Description: 检查MIC状态是否正常
  Author:      renjian
  Date:        2015-10-29
*******************************************************************************/
int testMIC()
{
	if (CON_MES_ENABLE)
	{
		if (DEVID_GET)
		{
			if (!mic->CheckDevID(devid))
			{
				g_Dlg->updateCurState(">>无法识别devid");
				goto MIC_CHECK_FAIL;
			}

			if (CON_ROUTE_CHECK_ENABLE)
			{
				if (!CON_ROUTE_CHECK_RESULT)
				{
					g_Dlg->updateCurState(">>流程检查未通过");
					goto MIC_CHECK_FAIL;
				}

				g_Dlg->updateCurState(">>MIC检查通过");
				return TRUE;
			}
			else
			{
				g_Dlg->updateCurState(">>跳过流程检查");
				return TRUE;
			}
		}
		else
		{
			g_Dlg->updateCurState(">>还未获得devid");
			goto MIC_CHECK_FAIL;
		}
	}
	else
	{
		g_Dlg->updateCurState(">>跳过MIC");
		return TRUE;
	}

MIC_CHECK_FAIL:
	g_Dlg->updateResult("  MIC未通过", RGB(255,0,0));
	return FALSE;
}

int testType()
{
	if (CON_THROUGH_TEST == 0 && CON_AUDIO_TEST == 0)
	{
		g_Dlg->updateResult("无可用测试项", RGB(255,0,0));
		g_Dlg->updateCurState(">>无可测试项");
		AfxMessageBox("请至少选择一个测试项");
		return FALSE;
	}

	return TRUE;
}



/*******************************************************************************
  Function:    getDBProductInfo
  Description: 从数据库中检查MAC地址及其他信息写入情况
  Author:      liguoliang
  Date:        2013-12-18
*******************************************************************************/
int getDBProductInfo()
{

	ULONG  Mac[2] = {0, 0};
	ULONG  MacLen = 6;

	g_Dlg->updateResult("查询写码信息", RGB(0,0,0));
	g_Dlg->updateCurState(">>获取MAC地址");

	//保存MAC记录
	SendARP(inet_addr(CON_DUTIP), 0, Mac, &MacLen);
	DUTMAC.Format("%02x-%02x-%02x-%02x-%02x-%02x",
		(Mac[0] & 0xff), (Mac[0] >> 8) & 0xff, (Mac[0] >> 16) & 0xff,
		(Mac[0] >> 24) & 0xff, Mac[1] & 0xff, (Mac[1] >> 8) & 0xff);
	DUTMACSHORT.Format("%02x%02x%02x%02x%02x%02x",
		(Mac[0] & 0xff), (Mac[0] >> 8) & 0xff, (Mac[0] >> 16) & 0xff,
		(Mac[0] >> 24) & 0xff, Mac[1] & 0xff, (Mac[1] >> 8) & 0xff);
	DUTMAC.MakeUpper();
	DUTMACSHORT.MakeUpper();

	char buf[20];
	sprintf(buf,"%s",DUTMAC);

	// 更新MAC显示
	g_Dlg->drawMACResult(RGB(0,0,0),buf);

	int exist = 0;

	if (CON_DBConnect == 0)
	{
		strcpy(productDBInfo.product_mac, DUTMACSHORT);
		return TRUE;
	}


	g_Dlg->updateCurState(">>查询是否写码成功");

	// 从数据库中查询是否包含指定的MAC
	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	sql.Format("select * from Product where p_mac = '%s' and p_model= %d",
				DUTMACSHORT, dev_model_id);
	//AfxMessageBox(sql);
	_RecordsetPtr m_pRecordset;
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// 不存在MAC，返回
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			exist = 0;
		}
		else
		{
			exist = 1;

			product_id = m_pRecordset->GetCollect("p_id");

			// 接下来从数据库中读取写入产品的其他信息
			strcpy(productDBInfo.product_mac, DUTMACSHORT);
			strcpy(productDBInfo.product_imei,(char*)(_bstr_t)m_pRecordset->GetCollect("p_imei"));
			strcpy(productDBInfo.product_meid,(char*)(_bstr_t)m_pRecordset->GetCollect("p_meid"));
			strcpy(productDBInfo.product_pin,(char*)(_bstr_t)m_pRecordset->GetCollect("p_pin"));
			strcpy(productDBInfo.product_sn,(char*)(_bstr_t)m_pRecordset->GetCollect("p_sn"));

			strcpy(productDBInfo.product_sw_ver, CON_SoftVer);
			strcpy(productDBInfo.product_hw_ver, CON_HardVer);

		}

		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");

		g_Dlg->updateCurState(">>传入数据失败");
		m_AdoConn.ExitConnect();
		return FALSE;
	}



	if (exist == 1)
	{
		return TRUE;
	}
	else
	{

		g_Dlg->updateCurState(">>数据库不存在相关产品信息>>停止");
		g_Dlg->updateResult(" 不存在产品信息", RGB(255,0,0));
		AfxMessageBox("写码工具并没有写入该MAC地址对应的产品信息！");

		return FALSE;
	}

}


/*******************************************************************************
  Function:    checkExistDBLog
  Description: 根据MAC查询是否已经存在对应的产品的测试记录
  Author:      liguoliang
  Date:        2013-12-19
*******************************************************************************/
int checkExistDBLog()
{
	if (CON_DBConnect == 0)
	{
		return FALSE;
	}

	g_Dlg->updateResult("查询测试记录", RGB(0,0,0));
	g_Dlg->updateCurState(">>查询是否已存在测试成功记录");

	// 频段信息
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	Flag=0;
	ULONG tl_id = 0;

	// 查询测试结果中是否已经存在该MAC所对应的设备的信息
	sql.Format("select * from ThroughputLog where tl_pid = %ld and tl_result=1 and tl_band = %ld",product_id, band_rec);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		if(m_AdoConn.m_pRecordset->adoEOF != 0)
		{
			Flag = 0;
		} else
		{
			Flag = 1;
			tl_id = m_pRecordset->GetCollect("tl_id");
		}

		m_AdoConn.ExitConnect();
	} catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");
		g_Dlg->updateCurState(">>传入数据失败");
		m_AdoConn.ExitConnect();
		return FALSE;
	}


	if (1 == Flag)
	{
		g_Dlg->updateCurState(">>存在相同MAC产品的测试成功记录>>停止");
		g_Dlg->updateResult(" 已存在测试记录", RGB(255,0,0));

		// 让操作人员可以选择改变原来测试记录
		if (IDOK == AfxMessageBox("数据库中已经包含该MAC地址的测试合格记录\n是否选择更改记录并重新测试?", MB_OKCANCEL))
		{
			changeDBPassLog(tl_id);
			return FALSE;
		}

		return TRUE;
	} else
	{
		return FALSE;
	}

}


/*******************************************************************************
  Function:    changeDBProductStatus
  Description: 修改数据库中Product表的代表测试结果的p_status字段的值
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBProductStatus(int success_or_failed)
{
	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	// 修改Product表的p_status值
	sql.Format("select * from Product where p_id = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// 不存在MAC，直接插入
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			AfxMessageBox("数据库中找不到产品信息！");
		} else
		{
			int status = m_pRecordset->GetCollect("p_status");

			if (1 == success_or_failed)
			{
				status = (status|2); // 设置该字段的值的二进制的第1位为1
			}
			else if (0 == success_or_failed)
			{
				status&=~(1<<1); // 设置该字段的值的二进制的第1位为0
			}

			m_pRecordset->PutCollect("p_status",(_variant_t)(status));

			m_pRecordset->Update();
		}
		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");

		g_Dlg->updateCurState(">>传入数据失败");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	return TRUE;
}


/*******************************************************************************
  Function:    deleteDBPassLog
  Description: 修改原来测试成功的记录并更改Product表的status字段
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBPassLog(ULONG tl_id)
{
	// 频段信息
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	// 修改Product表的p_status值
	sql.Format("select * from ThroughputLog where tl_id = %ld and tl_pid= %ld and tl_band = %ld",
				tl_id, product_id, band_rec);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// 不存在MAC，直接插入
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			AfxMessageBox("数据库中找不到测试记录！");
		} else
		{

			m_pRecordset->PutCollect("tl_result",(_variant_t)(0));
			m_pRecordset->PutCollect("tl_configResult", (_bstr_t)"重新测试");
			m_pRecordset->PutCollect("tl_userno",(_bstr_t)nb);
			m_pRecordset->PutCollect("tl_time",(_bstr_t)curTime.Format("%Y-%m-%d %H:%M:%S"));
			m_pRecordset->Update();
		}
		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("数据传输失败，请重新检查网络设置！");

		g_Dlg->updateCurState(">>传入数据失败");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	changeDBProductStatus(0);
	return TRUE;
}

/*******************************************************************************
  Function:    iperfTest
  Description: iperf测试工具接口
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void iperfTest()
{
	DWORD exitIperfPcThread = 0;
	DWORD exitIperfDutThread = 0;
	CON_FORP = 2;

	g_Dlg->updateResult("正在测试吞吐量", RGB(0,0,0));
	g_Dlg->updateCurState(">>正在测试吞吐量");

	// 如果按下“停止”键
	if (runState.stop == TRUE)
	{
		g_Dlg->updateCurState(">>停止");
		g_Dlg->updateResult("		  停止", RGB(0,0,0));

		system("TASKKILL /F /IM iperf.exe /T");

		return;
	}

	/* start pc iperf thread */
	iperfPcThread = CreateThread(NULL, 0, iperfPCThread, NULL, 0, NULL);

	/* start dut iperf thread */
	iperfDutThread = CreateThread(NULL, 0, iperfDUTThread, NULL, 0, NULL);

	while(CLOSE_PING_DUT != runState.pingDUT && IPERF_WORK_ERR != runState.iperfStatus)
	{
		GetExitCodeThread(iperfPcThread, &exitIperfPcThread);
		GetExitCodeThread(iperfDutThread, &exitIperfDutThread);

		if (STILL_ACTIVE != exitIperfPcThread && STILL_ACTIVE != exitIperfDutThread)
			break;

	}

	/* close iperf thread */
	if (iperfDutThread != NULL)
	{
        CloseHandle(iperfDutThread);
        iperfDutThread = NULL;
	}

	if (iperfPcThread != NULL)
	{
        CloseHandle(iperfPcThread);
        iperfPcThread = NULL;
	}

}

/*******************************************************************************
  Function:    chariotTest
  Description: chariot测试工具接口
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void chariotTest()
{
	char  starttime[65];
	char endtime[65];
	CString str;
	CTime tm;
	throughput_msg_str = "";
	throughput_value_str = "";
	int throughput_threshold= 0;

	double testValue = 0.0;

	CON_FORP = 2;

	//获取系统时间
	//CTime tm;
	tm=CTime::GetCurrentTime();
	str=tm.Format("%Y/%m/%d/ %X");
	sprintf(starttime, "%s", str);

	////////////////////////////////////////////////////////////////////////////
	//合格则只测一次即退出该循环，不合格的话再测试（目前是只测试一次，不合格即退出）
	////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < TEST_NUM; i++)
	{
		g_Dlg->updateResult("正在测试吞吐量", RGB(0,0,0));
		g_Dlg->updateCurState(">>正在测试吞吐量");

		/* 调用测吞吐量的函数，该函数返回平均值，若在测试过程中停止了，则返回的是测试时间内
		 * 的平均值
		 */

		// 调用真正实现吞吐量测试的函数
		avgThroughput[i] = beginThroughputTest();

		testValue = avgThroughput[i];

		// 如果按下“停止”键
		if (runState.stop == TRUE)
		{
			g_Dlg->updateCurState(">>停止");
			g_Dlg->updateResult("         停止", RGB(0,0,0));

			return;
		}

		throughput_msg_str.Format("平均吞吐量为：%.3f Mbps", avgThroughput[i]);
		throughput_value_str.Format("%.3f",avgThroughput[i]);
		SetDlgItemText(g_Dlg->hWnd, IDC_AVG, throughput_msg_str);

		// 保存测试结果到log中
		saveFileLog(throughput_value_str, curTime);

		enum WiFiBand band = product->getWiFiBand();
		if (band == BAND_2_GHZ) {
			throughput_threshold = CON_passAvg_24g;
		} else if (band == BAND_5_GHZ) {
			throughput_threshold = CON_passAvg_5g;
		} else {
			g_Dlg->updateCurState(">>BAND获取失败");
			g_Dlg->updateResult(" BAND获取失败", RGB(0,0,0));
			return;
		}

		// 如果测试结果小于预设速率值
		if (avgThroughput[i] < throughput_threshold)
		{
			throughputNoPass(throughput_value_str);

			continue;;
		}

		throughputPass(throughput_value_str);

		break;

	} // 结束大循环

	tm=CTime::GetCurrentTime();
	str=tm.Format("%Y/%m/%d/ %X");
	sprintf(endtime, "%s", str);

	if (CON_MES_ENABLE)
	{
		if (uploadData(throughput_value_str, starttime, endtime, throughput_threshold) < 0)
		{
			CON_FORP = 0;
		}
	}

	insertTestRecord(testValue, throughput_threshold);
	g_Dlg->m_RecordList.DeleteItem(20);

	return;

}

/*******************************************************************************
  Function:    autioTest
  Description: 音频测试工具接口
  Author:      renjian
  Date:        2016-05-24
*******************************************************************************/
void audioTest()
{
	g_Dlg->updateResult("正在测试音频", RGB(0,0,0));
	g_Dlg->updateCurState(">>正在测试音频");

	DWORD exitAudioResultThread = 0;

	CON_AUDIO_RESULT = 2;

	/* del cache file */
	system("cd .\\dist-audio\\cache && del/s/q *.*");

	Sleep(1000);

	/* start audio test */
	system("cd .\\dist-audio && start /b main.exe");

	audioResultThread = CreateThread(NULL, 0, audioResult, NULL, 0, NULL);

	while(CLOSE_PING_DUT != runState.pingDUT)
	{
		GetExitCodeThread(audioResultThread, &exitAudioResultThread);

		if (STILL_ACTIVE != exitAudioResultThread)
		{
			break;
		}
	}

	/* close audio thread */
	if (audioResultThread != NULL)
	{
		CloseHandle(audioResultThread);
		audioResultThread = NULL;
	}
}


/*******************************************************************************
  Function:    audioListen
  Description: 音频播放接口
  Author:      renjian
  Date:        2016-05-31
*******************************************************************************/
void audioListen()
{
	system("cd .\\dist-audio\\cache && start /b gzd_cache.wav");
}



/*******************************************************************************
  Function:    fixBandTo40
  Description: 设置固定信道，并将信道带宽设置为40M
  channel :0(自动信道) 1-13 (固定信道)
  bandwidth 1:20M  2:40M
  Author:      renjian
  Date:        2015-01-07
*******************************************************************************/
int fixBandTo40(int channel, int bandwidth)
{
	int ret = FALSE;
	ret = product->SetBandTo40(channel, bandwidth);

	return ret;
}

/*******************************************************************************
  Function:    getDevInfo
  Description: 获取设备信息
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int getDevInfo()
{
	g_Dlg->updateCurState(">>正在获取设备信息");
	int try_times = 5;
	int is_old_ver = 0;
	int ret = FALSE;

	product->initial = 0;
	strcpy(productWebInfo.product_sw_ver, product->getSoftWareVersion());

	while (try_times>0)
	{
		if ((0 != strcmp(productWebInfo.product_sw_ver, "N/A")))
		{
			ret = TRUE;
			break;
		}
		try_times--;

		// 老软件版本号获取方式
		if (is_old_ver == 0)
		{
			Sleep(1000);
			product->initialInfoItem();
			strcpy(productWebInfo.product_sw_ver, product->getSoftwareVerOld());
			is_old_ver = 1;
		} else
		{
			Sleep(1000);
			product->initial = 0;
			strcpy(productWebInfo.product_sw_ver, product->getSoftWareVersion());
			is_old_ver = 0;
		}
	}

	if (is_old_ver == 1)
	{
		strcpy(productWebInfo.product_hw_ver, product->getHardwareVerOld());
	} else
	{
		strcpy(productWebInfo.product_hw_ver, product->getHardWareVersion());
	}

	if (is_old_ver == 1)
	{
		strcpy(productWebInfo.product_mac, product->getMACOld());
	} else
	{
		strcpy(productWebInfo.product_mac, product->getMAC());
	}

	try_times = 5;
	if (is_old_ver == 1)
	{
		strcpy(productWebInfo.product_imei, product->getIMEIOld());
	} else
	{
		strcpy(productWebInfo.product_imei, product->getIMEI());

	}

	try_times = 5;
	if (is_old_ver == 1)
	{
		strcpy(productWebInfo.product_meid, product->getMEIDOld());
	} else
	{
		strcpy(productWebInfo.product_meid, product->getMEID());
	}

	ret = TRUE;
	if ((1 == prodcut_info_write.has_imei) &&
		(0 == strcmp(productWebInfo.product_imei, "N/A")))
	{
		//ret = FALSE;
		while (try_times>0)
		{
			try_times--;
			Sleep(2000);
			product->initial = 0;
			strcpy(productWebInfo.product_imei, product->getIMEI());

			if ((0 != strcmp(productWebInfo.product_imei, "N/A")))
			{
				ret = TRUE;
				break;
			}
		}

	}

	if ((1 == prodcut_info_write.has_meid) &&
		(0 == strcmp(productWebInfo.product_meid, "N/A")))
	{
		//ret = FALSE;
		while (try_times>0)
		{
			try_times--;
			Sleep(2000);
			product->initial = 0;
			strcpy(productWebInfo.product_meid, product->getMEID());

			if ((0 != strcmp(productWebInfo.product_meid, "N/A")))
			{
				ret = TRUE;
				break;
			}
		}
	}

	strcpy(productWebInfo.product_pin, product->getPIN());

	strcpy(productWebInfo.product_work_mode, product->getWorkingMode());

	strcpy(productWebInfo.product_modem_status, product->getModemStatus());

	strcpy(productWebInfo.product_devid, product->getDevid());

quit:
	g_Dlg->updateCurState(">>获取设备信息完毕");

	return ret;
}

/*******************************************************************************
  Function:    clearDevInfo
  Description: 清空设备信息
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
int clearDevInfo()
{
	strcpy(productWebInfo.product_sw_ver, "暂无信息");
	strcpy(productWebInfo.product_hw_ver, "暂无信息");
	strcpy(productWebInfo.product_mac, "暂无信息");
	strcpy(productWebInfo.product_imei, "暂无信息");
	strcpy(productWebInfo.product_meid, "暂无信息");
	strcpy(productWebInfo.product_pin, "暂无信息");
	strcpy(productWebInfo.product_work_mode, "暂无信息");
	strcpy(productWebInfo.product_modem_status, "暂无信息");
	strcpy(productWebInfo.product_devid, "暂无信息");

	g_Dlg->SetDlgItemText(IDC_AVG, "");
	g_Dlg->SetDlgItemText(IDC_CURSTATE, "");

	return TRUE;
}

/*******************************************************************************
  Function:    openWiFiThread
  Description: 一直开启WiFi知道停止为止
  Author:      liguoliang
  Date:        2013-12-25
*******************************************************************************/
DWORD WINAPI openWiFiThread(LPVOID pParam)
{
	int is_open = 0;
	int is_sync = 0;
	int ret = 0;

	g_Dlg->strCurState = "";
	g_Dlg->hWnd = g_Dlg->GetSafeHwnd();

	sockaddr_in serveraddr = {0};
	sockaddr_in clientaddr = {0};
	char resultBuf[MAXBUFFSIZE] = {0};

	// client
	SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if(!initSocket(&client_socket, &clientaddr, &serveraddr, CON_DUTIP))
	{
		g_Dlg->updateResult("  初始化失败", RGB(255,0,0));
		return FALSE;
	}

	char response[1024];
	char key [64];
	strcpy(key, CON_DUTUSER);
	strcat(key, CON_DUTPASSWORD);

	while(FALSE == runState.stop)
	{
		if (FALSE == myPing(CON_DUTIP))
		{
			is_open = 0;
			is_sync = 0;
			g_Dlg->updateResult("等待ping通设备", RGB(0,0,0));
			Sleep(2000);
		} else
		{
			if (0 == is_sync)
			{
				ret = tddpSync(&client_socket, &serveraddr, &clientaddr, key, response);

				if (ret == MSG_ERROR)
				{
					g_Dlg->updateResult("同步失败", RGB(255,0,0));
					Sleep(1000);
				} else
				{
					g_Dlg->updateResult("同步成功", RGB(0,255,0));
					is_sync = 1;
				}
			}
			else
			{
				if (0 == is_open)
				{
					g_Dlg->updateResult("正在打开WiFi", RGB(0,0,0));
					ret = controlWiFi(&client_socket, &serveraddr, &clientaddr, key, response, 1);

					if (ret == MSG_ERROR)
					{
						g_Dlg->updateResult("打开WiFi失败", RGB(255,0,0));
					} else
					{
						Sleep(10000);
						g_Dlg->updateResult("打开WiFi成功", RGB(0,255,0));
						is_open = 1;
					}
				} else
				{
					Sleep(1000);
				}
			}

		}

		Sleep(1000);
	}

	closesocket(client_socket);
	return TRUE;
}

/*******************************************************************************
  Function:    connectDUTThread
  Description: 连接设备线程，该线程会在选择机型后一直运行检测设备
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
DWORD WINAPI connectDUTThread(LPVOID pParam)
{
	while(CLOSE_PING_DUT != runState.pingDUT) // 退出程序
	{
		WaitForSingleObject(hMutex_mic, INFINITE);
		if (BEGIN_PING_DUT == runState.pingDUT) // 确定选择的机型
		{
			if (TRUE == myPing(CON_DUTIP)) // ping设备成功
			{
				//if (TRUE == myPing(CON_remoteIP))
				{
					Sleep(1000);
					if (FALSE == DUT_CONNECT && TRUE == myPing(CON_DUTIP)) // 如果之前是处于没有连接的状态
					{
						g_Dlg->updateResult("正获取设备信息", RGB(0,0,0));

						// 通过web请求获取产品信息显示在主界面上
						int ret = getDevInfo();
						g_Dlg->updateDevicInfo();
						if (ret == TRUE)
						{
							// 对于LTE设备，获取WiFi Band
							if (CON_DevType == DEVICE_LTE_MIFI || CON_DevType ==
							DEVICE_LTE_MSM8909 || CON_DevType == DEVICE_CARDV)
								g_Dlg->updateWiFiBandInfo();

							DUT_CONNECT = TRUE;

							g_Dlg->updateResult("  已连接设备", RGB(0,255,0));
						}
						else
						{
							DUT_CONNECT = FALSE;
							g_Dlg->updateResult("  未连接设备", RGB(255,0,0));

							CON_HT40 = FALSE;
							CON_ROUTE_CHECK_RESULT = FALSE;  // 重连设备时重新进行流程检查初始化
							DEVID_GET = FALSE;
							runState.checkMIC = BEGIN_CHECK_MIC;
						}
					}
				}
				#if 0
				else
				{
					DUT_CONNECT = FALSE; // 修改DUT连接状态
					g_Dlg->updateResult("未连接对端PC", RGB(255,0,0));
					runState.stop = TRUE;
					g_Dlg->strCurState = "";

					// 清空设备信息
					product->initial = 0;
					clearDevInfo();
					g_Dlg->updateDevicInfo();
				}
				#endif

			} else
			{
				DUT_CONNECT = FALSE; // 修改DUT连接状态
				g_Dlg->updateResult("  未连接设备", RGB(255,0,0));
				runState.stop = TRUE;
				g_Dlg->strCurState = "";

				CON_HT40 = FALSE;

				CON_ROUTE_CHECK_RESULT = FALSE;  // 重连设备时重新进行流程检查初始化
				DEVID_GET = FALSE;
				runState.checkMIC = BEGIN_CHECK_MIC;

				// 清空设备信息
				product->initial = 0;
				clearDevInfo();
				g_Dlg->updateDevicInfo();
			}
		} else if(STOP_PING_DUT == runState.pingDUT) // 修改选择
		{
			runState.stop = TRUE;
			DUT_CONNECT = FALSE; // 修改DUT连接状态
			g_Dlg->updateResult("  未选择设备", RGB(0,0,0));
			g_Dlg->strCurState = "";
			runState.stop = TRUE;

			CON_HT40 = FALSE;

			CON_ROUTE_CHECK_RESULT = FALSE;  // 重连设备时重新进行流程检查初始化
			DEVID_GET = FALSE;
			runState.checkMIC = BEGIN_CHECK_MIC;

			// 清空设备信息
			clearDevInfo();
			g_Dlg->updateDevicInfo();
		}

		ReleaseMutex(hMutex_mic);
		Sleep(1000);
	}
	return 1;
}


/*******************************************************************************
  Function:    connectMICThread
  Description: 连接MIC服务器线程，该线程会在选择机型后一直运行检测与MIC服务器的连接状态
  Author:      renjian
  Date:        2015-10-20
*******************************************************************************/
DWORD WINAPI connectMICThread(LPVOID pParam)
{
	int DEVID_GET_NUM = 5;
	int ROUTE_CHECK_NUM = 5;
	CString strButton;
	mic = new CWSDataTransCpp(CON_MIC_SERVER);
	g_Dlg->hWnd = g_Dlg->GetSafeHwnd();
	while(CLOSE_PING_DUT != runState.pingDUT)
	{
		if (BEGIN_PING_DUT == runState.pingDUT)
		{
			WaitForSingleObject(hMutex_mic, INFINITE);
			g_Dlg->GetDlgItem(IDC_MIC)->ShowWindow(SW_SHOW);
			Sleep(2000);
			if (mic->ConnectCheck())
			{
				MIC_CONNECT = TRUE;
				SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "已连接MIC");
			}
			else
			{
				MIC_CONNECT = FALSE;
				SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "未连接MIC");
				ReleaseMutex(hMutex_mic);
				continue;
			}

			// 获取devid
			if (CON_MES_ENABLE && TRUE == DUT_CONNECT)
			{
				//g_Dlg->updateResult("正获取devid", RGB(0,0,0));
				// TODO
				// 获取devid
				//devid = mic->ObtainDeviceID(CON_LINE, CON_STATION);
				//devid = mic->ObtainDeviceIDwithType(CON_LINE, CON_STATION, NX_OUTDOORAP);
				int devid_get_num = 0;
				while (!DEVID_GET && DUT_CONNECT)
				{
					devid = productWebInfo.product_devid;
					// 检查devid
					if (!mic->CheckDevID(devid))
					{
						DEVID_GET = FALSE;
						devid_get_num++;
						if (devid_get_num >= DEVID_GET_NUM)
						{
							//g_Dlg->updateCurState(">>获取DEVID失败");
							g_Dlg->updateResult("  未获取DEVID", RGB(255,0,0));
							runState.checkMIC = CHECK_MIC_FAIL;
							break;
							//runState.pingDUT = PAUSE_PING_DUT;
						}
					}
					else
					{
						DEVID_GET = TRUE;
						g_Dlg->updateCurState(">>获取DEVID完毕");
					}
				}

				if (CON_ROUTE_CHECK_ENABLE && !CON_ROUTE_CHECK_RESULT)
				{
					//int len;
					//char **mac = mic->GetDevidByMac("0000000011D0", &len);
					//g_Dlg->updateResult("正流程检查", RGB(0,0,0));
					int route_check_num = 0;
					while(!CON_ROUTE_CHECK_RESULT && DUT_CONNECT && CHECK_MIC_FAIL != runState.checkMIC)
					{
						int route_check_result = mic->RoutingCheck("T40", CON_CHECK_STATION, devid, "");
						switch (route_check_result)
						{
						case OK:
							//SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "流程检查通过");
							g_Dlg->updateCurState(">>流程检查完毕");
							CON_ROUTE_CHECK_RESULT= TRUE;
							runState.checkMIC = CHECK_MIC_SUCCESS;
							break;
						case ROUTING_CHECK_FAIL:
						case ROUTING_CHECK_NO_TEST:
						default:
							//SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "未查到上一站测试数据");
							CON_ROUTE_CHECK_RESULT = FALSE;
							route_check_num++;
							if (route_check_num >= ROUTE_CHECK_NUM)
							{
								//g_Dlg->updateCurState(">>流程检查未通过");
								g_Dlg->updateResult("  未通过检查", RGB(255,0,0));
								runState.checkMIC = CHECK_MIC_FAIL;
								//runState.pingDUT = PAUSE_PING_DUT;
							}
							break;
						}
					}
				}
				else if (!CON_ROUTE_CHECK_ENABLE && !CON_ROUTE_CHECK_RESULT)
				{
					CON_ROUTE_CHECK_RESULT= TRUE;
					runState.checkMIC = CHECK_MIC_SUCCESS;
					g_Dlg->updateCurState(">>跳过流程检查");
				}

			}
			else
			{
				if (!mic->CheckDevID(devid))
				{
					DEVID_GET = FALSE;
					CON_ROUTE_CHECK_RESULT = FALSE;
				}
			}
#if 0
			else if (DUT_STATE_TESTED == DUTState && mic->ConnectCheck() && mic->IsBufMode)  // 如果吞吐量已测试完
			{
				mic->BufferDataUpload("T40", &length);
				mic->IsBufMode = FALSE;
				g_Dlg->updateCurState(">>缓存数据已上传");
			}
#endif
			ReleaseMutex(hMutex_mic);
		}

		/* When mic is not ok, test is not allowed */
		g_Dlg->GetDlgItemText(IDC_TESTTHROUGHPUT, strButton);
		if (strButton == _T("开始测试"))
		{
			if (runState.checkMIC != CHECK_MIC_SUCCESS)
			{
				((CButton*)g_Dlg->GetDlgItem(IDC_TESTTHROUGHPUT))->EnableWindow(FALSE);
			}
			else
			{
				((CButton*)g_Dlg->GetDlgItem(IDC_TESTTHROUGHPUT))->EnableWindow(TRUE);
			}
		}
	}

	//  MIC资源释放
	mic->Release();
	delete[] mic;
	return TRUE;
}

/*******************************************************************************
  Function:    mac_compare
  Description: 自己定义的mac比较方法
  Author:      liguoliang
  Date:        2014-09-05
*******************************************************************************/
int mac_compare(char* mac1, char* mac2)
{
	int ret = 0;

	char format_str1[13] = {0};
	char format_str2[13] = {0};

	int i,j;

	// 忽略大小写以及转换成固定格式
	for (i = 0,j = 0; i < 12;)
	{
		if (mac1[j] != ':' && mac1[j] != '-')
		{
			if (mac1[j] >= 'a' && mac1[j] <= 'f')
				format_str1[i++] = mac1[j++]-32;
			else
				format_str1[i++] = mac1[j++];
		}
		else
		{
			j++;
		}
	}

	for (i = 0, j = 0; i < 12;)
	{
		if (mac2[j] != ':' && mac2[j] != '-')
		{
			if (mac2[j] >= 'a' && mac2[j] <= 'f')
				format_str2[i++] = mac2[j++]-32;
			else
				format_str2[i++] = mac2[j++];
		}
		else
		{
			j++;
		}
	}

	return strcmp(format_str1, format_str2);
}

int setField(char buffer[], int bufsiz, const char * valstr)
{
	int len = strlen(valstr);
	memset(buffer, 0, bufsiz);
	if ((len+1) > bufsiz)
		return -1;
	else
	{
		memcpy(buffer, valstr, len);
		return 0;
	}
}

int uploadData(CString avgThroughput, char starttime[], char endtime[], int throughput_threshold)
{
	// 上传数据
	CT40Identity idn;
	CT40Parameter paras[1];
	int mac_length = 12;
	_get_version_info(_version_infos);
	CString title;
	title.Format("%s V%s", _version_infos.product_name, _version_infos.product_ver);

	if (1 != CON_FORP)
	{
		setField(idn.Result, sizeof(idn.Result), "F");
		if (0 == CON_FORP)
		{
			setField(idn.Failmsg, sizeof(idn.Failmsg), "测试未通过");
		}
		else
		{
			setField(idn.Failmsg, sizeof(idn.Failmsg), "未进行吞吐量测试");
		}
		idn.Failcode = T40_FAIL_PERF;
	}
	else
	{
		setField(idn.Result, sizeof(idn.Result), "P");

		if (!CON_ROUTE_CHECK_ENABLE)
		{
			setField(idn.Failmsg, sizeof(idn.Failmsg), "未进行流程检查");
		}

		idn.Failcode = T40_PASS;
	}

	//setField(idn.Deviceid, sizeof(idn.Deviceid), devid);
	if (strlen(productDBInfo.product_mac) == mac_length)
	{
		setField(idn.Macid, sizeof(idn.Macid), productDBInfo.product_mac);
	}
	setField(idn.Hardwareid, sizeof(idn.Hardwareid), "");  //  云字段
	setField(idn.Line, sizeof(idn.Line), CON_LINE);
	setField(idn.Workstation, sizeof(idn.Workstation), station_number);
	setField(idn.Userno, sizeof(idn.Userno), nb);
	setField(idn.Equipmentno, sizeof(idn.Equipmentno), CON_EQUIP);
	setField(idn.Starttime, sizeof(idn.Starttime), starttime);
	setField(idn.Endtime, sizeof(idn.Endtime), endtime);
	setField(idn.Ver, sizeof(idn.Ver), title);  //  软件版本

	// 测试数据赋值
	CString Threshold;
	Threshold.Format("%d", throughput_threshold);
	CString TestTime;
	TestTime.Format("%d", CON_TEST_TIME);
	//CString Throught_result;
	//Throught_result.Format("%f", avgThroughput);
	paras[0] = CT40Parameter();
	setField(paras[0].Mode, sizeof(paras[0].Mode), "11N");
	setField(paras[0].Channel, sizeof(paras[0].Channel), "0");

	setField(paras[0].Rssi, sizeof(paras[0].Rssi), "0");
	setField(paras[0].Threshold, sizeof(paras[0].Threshold), Threshold);
	//setField(paras[0].Scriptname, sizeof(paras[0].Scriptname), CON_testFilename);
	setField(paras[0].Scriptname, sizeof(paras[0].Scriptname), "");
	setField(paras[0].Chariottime, sizeof(paras[0].Chariottime), TestTime);

	// 性能测试数据
	if (CON_TEST_TYPE == IPERF_TX_TEST)
	{
		setField(paras[0].Tcp_down, sizeof(paras[0].Tcp_down), avgThroughput);
	}
	else if (CON_TEST_TYPE == IPERF_RX_TEST)
	{
		setField(paras[0].Tcp_up, sizeof(paras[0].Tcp_up), avgThroughput);
	}
	else
	{
		setField(paras[0].Uppairs, sizeof(paras[0].Uppairs), "5");
		setField(paras[0].Downpairs, sizeof(paras[0].Downpairs), "5");
		paras[0].AddReserved("throught_result", avgThroughput);
	}
#if 0
	// 连接判断
	bool connected = mic->ConnectCheck();
	if (connected)
	{
		//  数据上传设置为直传模式
		mic->IsBufMode = false;
	}
	else
	{
		//  数据上传设置为缓存模式
		mic->IsBufMode = true;
		g_Dlg->updateCurState(">>无法连接到MIC");
	}
#endif

	int rst = mic->DataUpload(idn, paras, 1);
	if (rst < 0)
	{
		g_Dlg->updateCurState(">>已上传至本地缓存");
	}
	else
	{
		g_Dlg->updateCurState(">>已上传至MIC服务器");
	}

#if 0
	if (mic->IsBufMode)
	{
		g_Dlg->updateCurState(">>已上传至本地缓存");
	}
	else
	{
		g_Dlg->updateCurState(">>已上传至MIC服务器");
	}

	// 在适当的时候，重传缓存的数据
	mic->BufferDataUpload("T40", &length);
#endif
    return rst;
}

