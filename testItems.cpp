/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   testItems.cpp

  Description: ���������ʵ�ֵĺ�������ԭ����ͬһ���ļ��ж���ĺ����������

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
// ·������ route ADD 172.0.0.0 MASK 255.0.0.0 172.28.130.1 METRIC 1 -p
extern char * devid;
static struct VersionInfos _version_infos;

CString throughput_msg_str;   // ��ʾ�������������������ֵ���
CString throughput_value_str; // ��ʾ�����������������ֵ

CString sql;                  // ���ݿ����sql���
ADOConn m_AdoConn;            // ���ݿ���������

CTime curTime;                // ��Բ�Ʒ���β��Ե�ʱ��

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
					g_Dlg->updateCurState(">>��Ƶ����ͨ��");
					g_Dlg->updateResult("��Ƶ����ͨ��", RGB(0,255,0));
					CON_AUDIO_RESULT = 1;
					((CButton *)g_Dlg->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(TRUE);
				}

				if (strstr(buf, Fail))
				{
					g_Dlg->updateCurState(">>��Ƶ���Բ�ͨ��");
					g_Dlg->updateResult("��Ƶ���Բ�ͨ��", RGB(255,0,0));
					CON_AUDIO_RESULT = 0;
					((CButton *)g_Dlg->GetDlgItem(IDC_AUDIO_LISTEN))->EnableWindow(TRUE);
				}

				if (strstr(buf, Error))
				{
					g_Dlg->updateCurState(">>��Ƶ���Դ���");
					g_Dlg->updateResult("��Ƶ���Դ���", RGB(255,0,0));
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
			str.Format("��ֹͣ����");
			m_Avg->SetWindowText(str);
			system("TASKKILL /F /IM main.exe /T");
			break;
		}

		Sleep(1000);
	}

	if (CON_AUDIO_RESULT == 2)
	{
		g_Dlg->updateCurState(">>��Ƶ����ʧ��");
		g_Dlg->updateResult("��Ƶ����ʧ��", RGB(255,0,0));
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

	//��ȡϵͳʱ��
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
			g_Dlg->updateCurState(">>����PC��iperf");
			testValue = product->startIperfPC(test_in_pc);
		}
		else if (IPERF_STOP == runState.iperfStatus)
		{
			break;
		}
		Sleep(1000);
		//ReleaseMutex(hMutex);
	}

	throughput_msg_str.Format("ƽ��������Ϊ��%.3f Mbps", testValue);
	throughput_value_str.Format("%.3f",testValue);
	SetDlgItemText(g_Dlg->hWnd, IDC_AVG, throughput_msg_str);

	// ������Խ����log��
	saveFileLog(throughput_value_str, curTime);

	enum WiFiBand band = product->getWiFiBand();
	if (band == BAND_2_GHZ) {
		throughput_threshold = CON_passAvg_24g;
	} else if (band == BAND_5_GHZ) {
		throughput_threshold = CON_passAvg_5g;
	} else {
		g_Dlg->updateCurState(">>BAND��ȡʧ��");
		g_Dlg->updateResult(" BAND��ȡʧ��", RGB(0,0,0));
		return TRUE;
	}

	// ������Խ��С��Ԥ������ֵ
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
			g_Dlg->updateCurState(">>����DUT��iperf");
			if (product->startIperfDUT(test_in_dut) != 0)
			{
				// TODO:
				g_Dlg->updateCurState(">>����DUT��iperfʧ��");
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
*�� �� ����   throughputTest
*ժ    Ҫ��   ����������������һ�β��ϸ����ٲ�һ�Σ�
*             �ڸú����У���Ҫ�ǵ���beginThroughputTest()
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-15
*��ʷ�汾��   2013-12-10 liguoliang
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

	// ����PASSͼ�����MAC��ʾ����
	g_Dlg->updatePassPic(0);
	g_Dlg->updateFailPic(0);
	g_Dlg->SetDlgItemText(IDC_DUTMAC,"");

	closeShield();//�ر�������

	Sleep(100);
	avgThroughput[0]= 0;
	if (0 == strcmp(StrResult, "         ֹͣ"))
	{
		g_Dlg->updateResult("", RGB(0,0,0));
	}

    runState.begin = FALSE;

	// ���ARP������DUT�ͶԷ�����
	if(FALSE == connectDev())
	{
		goto test_finish;
	}

	////////////////////////////////////////////////////////////////////////////
	//����������ǰ�Ƚ���һЩ�豸��Ϣ���ݵļ�飬�Լ�����������豸�������Ƿ������������������
	////////////////////////////////////////////////////////////////////////////

	// ��ȡ�豸MAC�������ݿ��еõ����豸��������Ϣ
	if (!getDBProductInfo())
	{
		goto test_finish;
	}

	// ����MAC��ѯ�Ƿ��Ѿ����ڶ�Ӧ�Ĳ��Լ�¼
	if (TRUE == checkExistDBLog())
	{
		goto test_finish;
	}

	// �ж�ҳ����ʾ����Ӳ���汾��MAC��IMEI/MEID����Ϣ���Ƿ�ƥ��
	if(!testVersion())
	{
		goto test_finish;
	}

	// WAN�ڲ��ԣ�Ϊ�Ǳ��������
	if(!testWAN())
	{
		goto test_finish;
	}

	// PIN����ԣ�Ϊ�Ǳ��������
	if(!testPIN())
	{
		goto test_finish;
	}

	// ��ʽ���û��̣�Ϊ�Ǳ��������
	if(!formatDUT())
	{
		goto test_finish;
	}

	// 3G modem���䣬Ϊ�Ǳ��������
	if(!test3GModem())
	{
		goto test_finish;
	}

	// MIC ��飬Ϊ�Ǳ�����
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

	/* ֻ��������������ͨ�����߲���������ʱ���ܲ�����Ƶ */
	if (CON_FORP == 1 || CON_THROUGH_TEST == 0)
	{
		if (CON_AUDIO_TEST == 1)
		{
			audioTest();
		}
	}

test_finish:
	openShield();//��������

	if ((1 != CON_FORP && 1 == CON_THROUGH_TEST)
		|| (1 != CON_AUDIO_RESULT && 1 == CON_AUDIO_TEST))
	{
		failtimes++;  //��¼���ϸ����
		ft.Format("%d",failtimes);
		SetDlgItemText(g_Dlg->hWnd, IDC_FAILTIMES, ft);

		g_Dlg->updateFailPic(1);
		g_Dlg->updatePassPic(0);
	}
	else
	{
		//�ϸ�
		passtimes++;//��¼�ϸ����
		pt.Format("%d",passtimes);
		SetDlgItemText(g_Dlg->hWnd, IDC_PASSTIMES, pt);

		g_Dlg->updateFailPic(0);
		g_Dlg->updatePassPic(1);
	}

	float buffer = passtimes;	//ǿ��ת��Ϊfloat�ͣ��Ӷ���������ĳ�������
	passpro = buffer/(buffer+failtimes) *100;
	pp.Format("%.2f%s",passpro,"%");
	SetDlgItemText(g_Dlg->hWnd, IDC_PASSPRO, pp);

	g_Dlg->changeTestButton(STOP_TEST);
	DUTState = DUT_STATE_TESTED;

	return TRUE;
}

/*********************************************************
*�� �� ����   insertTestRecord
*ժ    Ҫ��   �����Լ�¼��ӵ���recordCount��Ԫ�ص�ѭ��������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-22
*********************************************************/
void insertTestRecord(double testValue, int throughput_threshold)
{
	char result[10];
	SYSTEMTIME localtime;
	GetLocalTime(&localtime);

	if (testValue >= throughput_threshold)
	{
		strcpy(result, " �ϸ� ");
	}
	else
	{
		strcpy(result, "���ϸ�");
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

	// [liguoliang start] �޸�ping�ķ�ʽ��ͨ������dll�еĺ��������
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
  Description: ���ø�ʽ���û��̵ĺ����Ļص�����
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
			// ��ʾ�ڽ�����
		} else {
			// ��ʾ�ڽ�����
		}
		break;
	}
	return TRUE;
}

// ��խ�ַ���ת���ɿ��ַ���
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
  Description: ���7Щ���д洢�̵ĵĻ��ͣ���ʽ���洢��
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
		for (int i=0;i<15;i++) //�ٶ������15������
		{
			if ((allDisk & 1)==1)
			{
				sprintf( diskPath, "%c", 'a'+i );
				strcat( diskPath, ":\\" );

				if ( GetDriveType(diskPath)==DRIVE_REMOVABLE )
				{
					char lpVolumeNameBuffer[32] = {0};

					if( GetVolumeInformation(diskPath,lpVolumeNameBuffer,32,0,0,0,0,0) ) //�ж������Ƿ�׼������
					{
						// ֻ�е��û��̱�ǩΪTP-LINK��ʱ��Ÿ�ʽ�������������U�̸�ʽ����
						if (strcmp(lpVolumeNameBuffer, "TP-LINK") == 0)
						{
							// ����fmifs.dll�ĺ���ִ�и�ʽ���Ĳ���
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
  Description: ����CMyDlg�е�AddError�����ɵ����ĺ���
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
void AddError(int er)
{
	//��ʱ���������ò��Խ����¼
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
		// [liguoliang start] ɾ���ظ�����

		//���������MAC��ֱ�Ӳ���
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
			g_Dlg->m_RecordList.SetItemText(0,1,"�ϸ�");
			CON_FORP = 1;
		}
		else if ( er == SOFTWARE_UNMATCH )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"����汾����");
			g_Dlg->m_RecordList.SetItemText(0,1,"����汾����");
		}
		else if ( er == HARDWARE_UNMATCH )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"Ӳ���汾����");
			g_Dlg->m_RecordList.SetItemText(0,1,"Ӳ���汾����");
		}
		else if ( er == PIN_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"PIN�����");
			g_Dlg->m_RecordList.SetItemText(0,1,"PIN�����");
		}
		else if ( er == MAC_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"MAC����");
			g_Dlg->m_RecordList.SetItemText(0,1,"MAC����");
		}
		else if ( er == WAN_ERROR )
		{
			m_pRecordset->PutCollect("tl_configResult",(_bstr_t)"WAN��ping��ͨ");
			g_Dlg->m_RecordList.SetItemText(0,1,"WAN��ping��ͨ");
		}

		m_pRecordset->Update();
		m_AdoConn1.ExitConnect();
		// [liguoliang end]

	}
	catch(...)
	{
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");
	}
}

/*******************************************************************************
  Function:    getMACInfo
  Description: ͨ������ARP����ȡMAC��ַ
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

	//����ARP��ѯ�����Զ��MAC��ַ
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
  Description: ����DUT�ͶԷ�����
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN connectDev()
{
	CString pingcmd;
	WinExec("arp -d", SW_HIDE);  // ���ARP��
	//pingcmd.Format("ping -n 1 -l 1000 %s",CON_DUTIP);
	//WinExec(pingcmd,SW_HIDE);

	//if (FALSE == pingRequest(CON_DUTIP))
	if (FALSE == DUT_CONNECT)
	{
		g_Dlg->updateCurState(">>DUTδ����");
		g_Dlg->updateResult("         ֹͣ", RGB(0,0,0));
		runState.stop = TRUE;
		return FALSE;
	}

	if (FALSE == MIC_CONNECT)
	{
		g_Dlg->updateCurState(">>MIC������δ����");  // δ�����¿��Լ�������
	}

	g_Dlg->updateCurState(">>DUT������");
	g_Dlg->updateResult("  ������", RGB(0,0,0));

	//�ȴ��ͶԷ�����pingͨ
	g_Dlg->updateCurState(">>�ȴ��ͶԷ���������");

	if (TRUE != pingRequest(CON_remoteIP))
	{
		g_Dlg->updateCurState(">>δ��������>>ֹͣ");
		g_Dlg->updateResult("         ֹͣ", RGB(0,0,0));

		return FALSE;
	}
	else
	{
		g_Dlg->updateCurState(">>�ѽ�������");
	}

	return TRUE;
}


/*******************************************************************************
  Function:    saveFileLog
  Description: �������������Խ����log�ļ���
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN saveFileLog(CString avg, CTime time)
{
	CString filename;
	filename.Format(".\\log\\%s.log",time.Format("%y%m%d"));
	FILE* fp;                 //��MAC��ַ��¼��log�ļ���
	fp=fopen(filename,"a");

	fprintf(fp,"%s %s ƽ��������Ϊ��%s Mbps ����Ա:%s\n",time.Format("%Hʱ%M��"),DUTMAC,avg,nb);
	fclose(fp);

	g_Dlg->m_RecordList.InsertItem(0,"",0);
	g_Dlg->m_RecordList.SetItemText(0,0,DUTMAC);
	g_Dlg->m_RecordList.SetItemText(0,2,avg);

	return TRUE;
}


/*******************************************************************************
  Function:    testWAN
  Description: ����WAN��
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testWAN()
{
	if( CON_WANTEST == 1 ) // WAN�ڼ��
	{
		g_Dlg->updateResult("����WAN��", RGB(0,0,0));
		DWORD k=pingWAN(CON_WANIP);
		if (k != TRUE)
		{
			error_no = WAN_ERROR;
			AddError(error_no);

			g_Dlg->updateCurState(">>WAN��ping��ͨ>>ֹͣ");
			g_Dlg->updateResult("WAN�ڲ���ʧ��", RGB(255,0,0));

			return FALSE;
		} else
		{
			g_Dlg->updateCurState(">>WAN�ڲ���ͨ��");
		}
	}
	return TRUE;
}


/*******************************************************************************
  Function:    resetDUT
  Description: �����λ
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int resetDUT()
{
	CString str;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);

	if ( CON_Reset == 1 ) //��λ
	{
		g_Dlg->updateResult("���ڸ�λ���", RGB(0,0,0));
		CString msg;
		msg.Format(">>���͸�λ���ݰ������õȴ�ʱ��Ϊ%d", CON_RESET_TIME/1000);
		g_Dlg->updateCurState(msg);

		int timeOut = product->reset();
		if(0 == timeOut)
		{
			AfxMessageBox("�����λʧ�ܣ��볢�Ե�¼����ҳ���ֶ����лָ���������");

			g_Dlg->updateResult("��λʧ��", RGB(255,0,0));
			g_Dlg->updateCurState(">>��λʧ��");

			return FALSE;
		} else
		{
			int time_second = timeOut/1000;
			for (int i = 0; i < time_second; i++)
			{
				str.Format("���ڸ�λ���ѹ� %d ��", i+1);
				m_Avg->SetWindowText(str);
				if(FALSE == myPing(CON_DUTIP))
				{
					Sleep(3000);
					break;
				}
				Sleep(1000);
			}
		}

		g_Dlg->updateResult("   ��λ���", RGB(0,0,0));
		g_Dlg->updateCurState(">>��λ�ɹ�");
	}

	return TRUE;
}

extern void c2w_utf8(wchar_t *pwstr,size_t len,const char *str);
extern void c2w_acp(wchar_t *pwstr,size_t len,const char *str);

/*******************************************************************************
  Function:    testVersion
  Description: �����Ӳ���汾����ҳ��ʾ��MAC��ַ�Ƿ�ƥ��ȹ���
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testVersion()
{
	if (0 == CON_DBConnect && DEVICE_CARDV != CON_DevType)
	{
		return TRUE;
	}

	if ( CON_CHECKVER == 1) // ���汾��Ϣ��MAC��ַ
	{
		g_Dlg->updateResult("��Ϣƥ����", RGB(0,0,0));

		//�˶�����汾��Ӳ���汾
		g_Dlg->updateCurState(">>�豸��Ϣƥ�����");

		// ����汾���
		if ( 0 != strcmp(productDBInfo.product_sw_ver, "N/A") && DEVICE_CARDV != CON_DevType)
		{
			if (NULL == strstr(productDBInfo.product_sw_ver, productWebInfo.product_sw_ver))
			{
				g_Dlg->updateResult("����汾����", RGB(255,0,0));
				g_Dlg->updateCurState(">>����汾����");

				AfxMessageBox("����汾����");
				return FALSE;
			}
		}

		if (0 != strcmp(CON_CHECK_SOFTWARE, "N/A"))
		{
			if (NULL == strstr(CON_CHECK_SOFTWARE, productWebInfo.product_sw_ver))
			{
				g_Dlg->updateResult("����汾����", RGB(255,0,0));
				g_Dlg->updateCurState(">>����汾����");

				AfxMessageBox("����汾����");
				return FALSE;
			}
		}

		// Ӳ���汾���
		if ( 0!= strcmp(productDBInfo.product_hw_ver, "N/A") && DEVICE_CARDV != CON_DevType)
		{
			/* TR961 2500 �ƶ�����Ŀ��ͳһ�ڻ�ȡ�豸��Ϣ�������unicode��
			ת�룬������������������
			*/
			if (DEVICE_LTE_CMCC == CON_DevType)
			{
				if (NULL == strstr(productDBInfo.product_hw_ver, productWebInfo.product_hw_ver))
				{
					g_Dlg->updateResult("Ӳ���汾����", RGB(255,0,0));
					g_Dlg->updateCurState(">>Ӳ���汾����");

					AfxMessageBox("Ӳ���汾����");
					return FALSE;
				}
			}
			else
			{
				// TODO:�Ƚ�wchar��char�������ַ���δ���
				wchar_t webinfo_hw_ver_unicode[100];
				wchar_t db_hw_ver_unicode[100];
				c2w_utf8(webinfo_hw_ver_unicode,100, productWebInfo.product_hw_ver);
				c2w_acp(db_hw_ver_unicode,100, productDBInfo.product_hw_ver);

				if (NULL == StrStrIW(webinfo_hw_ver_unicode, db_hw_ver_unicode))
				//if (NULL == strstr(productDBInfo.product_hw_ver, productWebInfo.product_hw_ver))
				{
					g_Dlg->updateResult("Ӳ���汾����", RGB(255,0,0));
					g_Dlg->updateCurState(">>Ӳ���汾����");

					AfxMessageBox("Ӳ���汾����");
					return FALSE;
				}
			}
		}

		if (0 != strcmp(CON_CHECK_HARDWARE, "N/A"))
		{
			if (NULL == strstr(CON_CHECK_HARDWARE, productWebInfo.product_hw_ver))
			{
				g_Dlg->updateResult("Ӳ���汾����", RGB(255,0,0));
				g_Dlg->updateCurState(">>Ӳ���汾����");

				AfxMessageBox("Ӳ���汾����");
				return FALSE;
			}
		}

		// MAC��ַ���

		if(((0 != product_test.test_mac) || DEVICE_CARDV == CON_DevType)  && (0 != mac_compare((LPSTR)(LPCTSTR)DUTMAC, productWebInfo.product_mac)))
		{
			g_Dlg->updateResult("��ҳMAC����",RGB(255,0,0));
			g_Dlg->updateCurState(">>��ҳMAC��ַ����");

			AfxMessageBox("��ҳMAC����");
			return FALSE;
		}

		// IMEI���
		/* TR961 2500 �ƶ�����Ŀ�й涨д�����ݿ�Ĳ��Ϸ�IMEI
		����16λ��ʾ
		*/
		if (DEVICE_LTE_CMCC == CON_DevType)
		{
			if((0 != product_test.test_imei) && (15 == strlen(productDBInfo.product_imei)))
			{
				if (0 != strcmp(productDBInfo.product_imei, productWebInfo.product_imei))
				{
					g_Dlg->updateResult("IMEI����", RGB(255,0,0));
					g_Dlg->updateCurState(">>IMEI����");

					AfxMessageBox("IMEI����");
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
					g_Dlg->updateResult("IMEI����", RGB(255,0,0));
					g_Dlg->updateCurState(">>IMEI����");

					AfxMessageBox("IMEI����");
					return FALSE;
				}
			}
		}

		// MEID���
		if ((0 != product_test.test_meid) && (0!= strcmp(productDBInfo.product_meid, "N/A")) && DEVICE_CARDV != CON_DevType)
		{
			if (0 != strcmp(productDBInfo.product_meid, productWebInfo.product_meid))
			{
				g_Dlg->updateResult("MEID����", RGB(255,0,0));
				g_Dlg->updateCurState(">>MEID����");

				AfxMessageBox("MEID����");
				return FALSE;
			}
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    testPIN
  Description: ���PIN��
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int testPIN()
{
	if (0 == CON_DBConnect)
	{
		return TRUE;
	}

	if ( CON_PINcheck == 1 )  //��鲢����PIN��
	{
		g_Dlg->updateResult("���PIN��", RGB(0,0,0));
		g_Dlg->updateCurState(">>���PIN��");

		// PIN����
		if ( 0 != strcmp(productDBInfo.product_pin, "N/A"))
		{
			if (0 != strcmp(productDBInfo.product_pin, productWebInfo.product_pin))
			{
				g_Dlg->updateResult("PIN�����", RGB(255,0,0));
				g_Dlg->updateCurState(">>PIN�����");

				AfxMessageBox("PIN��������");
				return FALSE;
			}
		}

	} // ����PIN�����

	return TRUE;
}


/*******************************************************************************
  Function:    throughputPass
  Description: ��¼�������ϸ�
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputPass(CString throughput_value)
{
	//�����Խ���������ݿ���
	error_no = PASS_NOERROR;  //�������

	g_Dlg->updateResult("����������ͨ��", RGB(0,255,0));
	g_Dlg->updateCurState(">>����������ͨ��");

	g_Dlg->m_RecordList.SetItemText(0,1,"�ϸ�");
	CON_FORP = 1;

	if (CON_DBConnect == 0)
	{
		return TRUE;
	}

	// Ƶ����Ϣ
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	g_Dlg->updateCurState(">>���������ݿ�>>���ڴ�������");

	_RecordsetPtr m_pRecordset;

	//��ԭ����ÿ��MAC�������Ϊ�ȼ�����������ͬMAC�ٲ���
	sql.Format("select * from ThroughputLog where tl_pid = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// ������MAC��ֱ�Ӳ���
		//if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			m_pRecordset->AddNew();
			g_Dlg->updateCurState(">>�����������(������MAC)");
		}
		//else // �������MAC��ֱ�Ӹ���
		//{
		//	strCurState += ">>�����������(MAC�Ѵ���)";
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
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");

		g_Dlg->updateCurState(">>��������ʧ��");
		return FALSE;
	}

	changeDBProductStatus(1);
	return TRUE;
}


/*******************************************************************************
  Function:    throughputNoPass
  Description: ��¼���������ϸ�
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputNoPass(CString throughput_value)
{
	g_Dlg->m_RecordList.SetItemText(0,1,"���ʵ�");

	g_Dlg->updateCurState(">>���ϸ�");
	g_Dlg->updateResult("���ʵ�", RGB(255,0,0));

	CON_FORP = 0;

	if (CON_DBConnect == 0)
	{
		return TRUE;
	}

	// Ƶ����Ϣ
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	g_Dlg->updateCurState(">>���������ݿ�>>���ڴ�������");

	CString sql;
	_RecordsetPtr m_pRecordset;

	//��ԭ����ÿ��MAC�������Ϊ�ȼ�����������ͬMAC�ٲ���
	sql.Format("select * from ThroughputLog where tl_pid = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		//���������MAC��ֱ�Ӳ���
		//if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			m_pRecordset->AddNew();
			g_Dlg->updateCurState(">>�����������(������MAC)");
		}
		/*else
		{
		strCurState += ">>�����������(MAC�Ѵ���)";
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
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");

		g_Dlg->updateCurState(">>��������ʧ��");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	return TRUE;
}


/*******************************************************************************
  Function:    formatDUT
  Description: ��ʽ���û���
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int formatDUT()
{
	if (CON_FormatDisk == 1) // ��ʽ���û���
	{
		g_Dlg->updateResult("��ʽ���û���",RGB(0,0,0));

		if(!formatDisk())
		{
			g_Dlg->updateCurState(">>�û��̸�ʽ��ʧ��>>ֹͣ");
			g_Dlg->updateResult(" ��ʽ��U��ʧ��", RGB(255,0,0));

			AfxMessageBox("��ʽ��ʧ�ܣ�������'TP-LINK'��ǩ���û���!");
		}
		else
		{
			g_Dlg->updateCurState(">>��ʽ���û��̳ɹ�");
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    test3GModem
  Description: ���3G Modem�Ƿ�����
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int test3GModem()
{
	if (CON_3GModem == 1) // 3G modemʶ��
	{
		g_Dlg->updateResult("���3G MODEM", RGB(0,0,0));

		if (0 == strcmp(productWebInfo.product_modem_status, "N/A"))
		{
			g_Dlg->updateCurState(">>�޷�ʶ��3G modem");
			AfxMessageBox("�޷�ʶ��3G Modem!");

			return FALSE;
		} else if (0 == strcmp(productWebInfo.product_modem_status, "Ready"))
		{
			g_Dlg->updateCurState(">>3G modem��׼��");
		} else if (0 == strcmp(productWebInfo.product_modem_status, "Not Ready"))
		{
			g_Dlg->updateCurState(">>3G modemδ׼��");
		} else
		{
			g_Dlg->updateCurState(">>3G modem״̬δ֪");
		}
	}

	return TRUE;
}


/*******************************************************************************
  Function:    testMIC
  Description: ���MIC״̬�Ƿ�����
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
				g_Dlg->updateCurState(">>�޷�ʶ��devid");
				goto MIC_CHECK_FAIL;
			}

			if (CON_ROUTE_CHECK_ENABLE)
			{
				if (!CON_ROUTE_CHECK_RESULT)
				{
					g_Dlg->updateCurState(">>���̼��δͨ��");
					goto MIC_CHECK_FAIL;
				}

				g_Dlg->updateCurState(">>MIC���ͨ��");
				return TRUE;
			}
			else
			{
				g_Dlg->updateCurState(">>�������̼��");
				return TRUE;
			}
		}
		else
		{
			g_Dlg->updateCurState(">>��δ���devid");
			goto MIC_CHECK_FAIL;
		}
	}
	else
	{
		g_Dlg->updateCurState(">>����MIC");
		return TRUE;
	}

MIC_CHECK_FAIL:
	g_Dlg->updateResult("  MICδͨ��", RGB(255,0,0));
	return FALSE;
}

int testType()
{
	if (CON_THROUGH_TEST == 0 && CON_AUDIO_TEST == 0)
	{
		g_Dlg->updateResult("�޿��ò�����", RGB(255,0,0));
		g_Dlg->updateCurState(">>�޿ɲ�����");
		AfxMessageBox("������ѡ��һ��������");
		return FALSE;
	}

	return TRUE;
}



/*******************************************************************************
  Function:    getDBProductInfo
  Description: �����ݿ��м��MAC��ַ��������Ϣд�����
  Author:      liguoliang
  Date:        2013-12-18
*******************************************************************************/
int getDBProductInfo()
{

	ULONG  Mac[2] = {0, 0};
	ULONG  MacLen = 6;

	g_Dlg->updateResult("��ѯд����Ϣ", RGB(0,0,0));
	g_Dlg->updateCurState(">>��ȡMAC��ַ");

	//����MAC��¼
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

	// ����MAC��ʾ
	g_Dlg->drawMACResult(RGB(0,0,0),buf);

	int exist = 0;

	if (CON_DBConnect == 0)
	{
		strcpy(productDBInfo.product_mac, DUTMACSHORT);
		return TRUE;
	}


	g_Dlg->updateCurState(">>��ѯ�Ƿ�д��ɹ�");

	// �����ݿ��в�ѯ�Ƿ����ָ����MAC
	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	sql.Format("select * from Product where p_mac = '%s' and p_model= %d",
				DUTMACSHORT, dev_model_id);
	//AfxMessageBox(sql);
	_RecordsetPtr m_pRecordset;
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// ������MAC������
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			exist = 0;
		}
		else
		{
			exist = 1;

			product_id = m_pRecordset->GetCollect("p_id");

			// �����������ݿ��ж�ȡд���Ʒ��������Ϣ
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
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");

		g_Dlg->updateCurState(">>��������ʧ��");
		m_AdoConn.ExitConnect();
		return FALSE;
	}



	if (exist == 1)
	{
		return TRUE;
	}
	else
	{

		g_Dlg->updateCurState(">>���ݿⲻ������ز�Ʒ��Ϣ>>ֹͣ");
		g_Dlg->updateResult(" �����ڲ�Ʒ��Ϣ", RGB(255,0,0));
		AfxMessageBox("д�빤�߲�û��д���MAC��ַ��Ӧ�Ĳ�Ʒ��Ϣ��");

		return FALSE;
	}

}


/*******************************************************************************
  Function:    checkExistDBLog
  Description: ����MAC��ѯ�Ƿ��Ѿ����ڶ�Ӧ�Ĳ�Ʒ�Ĳ��Լ�¼
  Author:      liguoliang
  Date:        2013-12-19
*******************************************************************************/
int checkExistDBLog()
{
	if (CON_DBConnect == 0)
	{
		return FALSE;
	}

	g_Dlg->updateResult("��ѯ���Լ�¼", RGB(0,0,0));
	g_Dlg->updateCurState(">>��ѯ�Ƿ��Ѵ��ڲ��Գɹ���¼");

	// Ƶ����Ϣ
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	Flag=0;
	ULONG tl_id = 0;

	// ��ѯ���Խ�����Ƿ��Ѿ����ڸ�MAC����Ӧ���豸����Ϣ
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
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");
		g_Dlg->updateCurState(">>��������ʧ��");
		m_AdoConn.ExitConnect();
		return FALSE;
	}


	if (1 == Flag)
	{
		g_Dlg->updateCurState(">>������ͬMAC��Ʒ�Ĳ��Գɹ���¼>>ֹͣ");
		g_Dlg->updateResult(" �Ѵ��ڲ��Լ�¼", RGB(255,0,0));

		// �ò�����Ա����ѡ��ı�ԭ�����Լ�¼
		if (IDOK == AfxMessageBox("���ݿ����Ѿ�������MAC��ַ�Ĳ��Ժϸ��¼\n�Ƿ�ѡ����ļ�¼�����²���?", MB_OKCANCEL))
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
  Description: �޸����ݿ���Product��Ĵ�����Խ����p_status�ֶε�ֵ
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBProductStatus(int success_or_failed)
{
	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	// �޸�Product���p_statusֵ
	sql.Format("select * from Product where p_id = %ld ",product_id);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// ������MAC��ֱ�Ӳ���
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			AfxMessageBox("���ݿ����Ҳ�����Ʒ��Ϣ��");
		} else
		{
			int status = m_pRecordset->GetCollect("p_status");

			if (1 == success_or_failed)
			{
				status = (status|2); // ���ø��ֶε�ֵ�Ķ����Ƶĵ�1λΪ1
			}
			else if (0 == success_or_failed)
			{
				status&=~(1<<1); // ���ø��ֶε�ֵ�Ķ����Ƶĵ�1λΪ0
			}

			m_pRecordset->PutCollect("p_status",(_variant_t)(status));

			m_pRecordset->Update();
		}
		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");

		g_Dlg->updateCurState(">>��������ʧ��");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	return TRUE;
}


/*******************************************************************************
  Function:    deleteDBPassLog
  Description: �޸�ԭ�����Գɹ��ļ�¼������Product���status�ֶ�
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBPassLog(ULONG tl_id)
{
	// Ƶ����Ϣ
	int band_rec = (int)(product->getWiFiBand());

	//&ADOConn::OnInitADOConn;
	m_AdoConn.OnInitADOConn();

	_RecordsetPtr m_pRecordset;
	CString sql;

	// �޸�Product���p_statusֵ
	sql.Format("select * from ThroughputLog where tl_id = %ld and tl_pid= %ld and tl_band = %ld",
				tl_id, product_id, band_rec);
	m_pRecordset=m_AdoConn.GetRecordSet((_bstr_t)sql);

	try
	{
		// ������MAC��ֱ�Ӳ���
		if(m_AdoConn.m_pRecordset->adoEOF!=0)
		{
			AfxMessageBox("���ݿ����Ҳ������Լ�¼��");
		} else
		{

			m_pRecordset->PutCollect("tl_result",(_variant_t)(0));
			m_pRecordset->PutCollect("tl_configResult", (_bstr_t)"���²���");
			m_pRecordset->PutCollect("tl_userno",(_bstr_t)nb);
			m_pRecordset->PutCollect("tl_time",(_bstr_t)curTime.Format("%Y-%m-%d %H:%M:%S"));
			m_pRecordset->Update();
		}
		m_AdoConn.ExitConnect();
	}
	catch(...)
	{
		AfxMessageBox("���ݴ���ʧ�ܣ������¼���������ã�");

		g_Dlg->updateCurState(">>��������ʧ��");
		m_AdoConn.ExitConnect();
		return FALSE;
	}

	changeDBProductStatus(0);
	return TRUE;
}

/*******************************************************************************
  Function:    iperfTest
  Description: iperf���Թ��߽ӿ�
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void iperfTest()
{
	DWORD exitIperfPcThread = 0;
	DWORD exitIperfDutThread = 0;
	CON_FORP = 2;

	g_Dlg->updateResult("���ڲ���������", RGB(0,0,0));
	g_Dlg->updateCurState(">>���ڲ���������");

	// ������¡�ֹͣ����
	if (runState.stop == TRUE)
	{
		g_Dlg->updateCurState(">>ֹͣ");
		g_Dlg->updateResult("		  ֹͣ", RGB(0,0,0));

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
  Description: chariot���Թ��߽ӿ�
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

	//��ȡϵͳʱ��
	//CTime tm;
	tm=CTime::GetCurrentTime();
	str=tm.Format("%Y/%m/%d/ %X");
	sprintf(starttime, "%s", str);

	////////////////////////////////////////////////////////////////////////////
	//�ϸ���ֻ��һ�μ��˳���ѭ�������ϸ�Ļ��ٲ��ԣ�Ŀǰ��ֻ����һ�Σ����ϸ��˳���
	////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < TEST_NUM; i++)
	{
		g_Dlg->updateResult("���ڲ���������", RGB(0,0,0));
		g_Dlg->updateCurState(">>���ڲ���������");

		/* ���ò��������ĺ������ú�������ƽ��ֵ�����ڲ��Թ�����ֹͣ�ˣ��򷵻ص��ǲ���ʱ����
		 * ��ƽ��ֵ
		 */

		// ��������ʵ�����������Եĺ���
		avgThroughput[i] = beginThroughputTest();

		testValue = avgThroughput[i];

		// ������¡�ֹͣ����
		if (runState.stop == TRUE)
		{
			g_Dlg->updateCurState(">>ֹͣ");
			g_Dlg->updateResult("         ֹͣ", RGB(0,0,0));

			return;
		}

		throughput_msg_str.Format("ƽ��������Ϊ��%.3f Mbps", avgThroughput[i]);
		throughput_value_str.Format("%.3f",avgThroughput[i]);
		SetDlgItemText(g_Dlg->hWnd, IDC_AVG, throughput_msg_str);

		// ������Խ����log��
		saveFileLog(throughput_value_str, curTime);

		enum WiFiBand band = product->getWiFiBand();
		if (band == BAND_2_GHZ) {
			throughput_threshold = CON_passAvg_24g;
		} else if (band == BAND_5_GHZ) {
			throughput_threshold = CON_passAvg_5g;
		} else {
			g_Dlg->updateCurState(">>BAND��ȡʧ��");
			g_Dlg->updateResult(" BAND��ȡʧ��", RGB(0,0,0));
			return;
		}

		// ������Խ��С��Ԥ������ֵ
		if (avgThroughput[i] < throughput_threshold)
		{
			throughputNoPass(throughput_value_str);

			continue;;
		}

		throughputPass(throughput_value_str);

		break;

	} // ������ѭ��

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
  Description: ��Ƶ���Թ��߽ӿ�
  Author:      renjian
  Date:        2016-05-24
*******************************************************************************/
void audioTest()
{
	g_Dlg->updateResult("���ڲ�����Ƶ", RGB(0,0,0));
	g_Dlg->updateCurState(">>���ڲ�����Ƶ");

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
  Description: ��Ƶ���Žӿ�
  Author:      renjian
  Date:        2016-05-31
*******************************************************************************/
void audioListen()
{
	system("cd .\\dist-audio\\cache && start /b gzd_cache.wav");
}



/*******************************************************************************
  Function:    fixBandTo40
  Description: ���ù̶��ŵ��������ŵ���������Ϊ40M
  channel :0(�Զ��ŵ�) 1-13 (�̶��ŵ�)
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
  Description: ��ȡ�豸��Ϣ
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int getDevInfo()
{
	g_Dlg->updateCurState(">>���ڻ�ȡ�豸��Ϣ");
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

		// ������汾�Ż�ȡ��ʽ
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
	g_Dlg->updateCurState(">>��ȡ�豸��Ϣ���");

	return ret;
}

/*******************************************************************************
  Function:    clearDevInfo
  Description: ����豸��Ϣ
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
int clearDevInfo()
{
	strcpy(productWebInfo.product_sw_ver, "������Ϣ");
	strcpy(productWebInfo.product_hw_ver, "������Ϣ");
	strcpy(productWebInfo.product_mac, "������Ϣ");
	strcpy(productWebInfo.product_imei, "������Ϣ");
	strcpy(productWebInfo.product_meid, "������Ϣ");
	strcpy(productWebInfo.product_pin, "������Ϣ");
	strcpy(productWebInfo.product_work_mode, "������Ϣ");
	strcpy(productWebInfo.product_modem_status, "������Ϣ");
	strcpy(productWebInfo.product_devid, "������Ϣ");

	g_Dlg->SetDlgItemText(IDC_AVG, "");
	g_Dlg->SetDlgItemText(IDC_CURSTATE, "");

	return TRUE;
}

/*******************************************************************************
  Function:    openWiFiThread
  Description: һֱ����WiFi֪��ֹͣΪֹ
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
		g_Dlg->updateResult("  ��ʼ��ʧ��", RGB(255,0,0));
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
			g_Dlg->updateResult("�ȴ�pingͨ�豸", RGB(0,0,0));
			Sleep(2000);
		} else
		{
			if (0 == is_sync)
			{
				ret = tddpSync(&client_socket, &serveraddr, &clientaddr, key, response);

				if (ret == MSG_ERROR)
				{
					g_Dlg->updateResult("ͬ��ʧ��", RGB(255,0,0));
					Sleep(1000);
				} else
				{
					g_Dlg->updateResult("ͬ���ɹ�", RGB(0,255,0));
					is_sync = 1;
				}
			}
			else
			{
				if (0 == is_open)
				{
					g_Dlg->updateResult("���ڴ�WiFi", RGB(0,0,0));
					ret = controlWiFi(&client_socket, &serveraddr, &clientaddr, key, response, 1);

					if (ret == MSG_ERROR)
					{
						g_Dlg->updateResult("��WiFiʧ��", RGB(255,0,0));
					} else
					{
						Sleep(10000);
						g_Dlg->updateResult("��WiFi�ɹ�", RGB(0,255,0));
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
  Description: �����豸�̣߳����̻߳���ѡ����ͺ�һֱ���м���豸
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
DWORD WINAPI connectDUTThread(LPVOID pParam)
{
	while(CLOSE_PING_DUT != runState.pingDUT) // �˳�����
	{
		WaitForSingleObject(hMutex_mic, INFINITE);
		if (BEGIN_PING_DUT == runState.pingDUT) // ȷ��ѡ��Ļ���
		{
			if (TRUE == myPing(CON_DUTIP)) // ping�豸�ɹ�
			{
				//if (TRUE == myPing(CON_remoteIP))
				{
					Sleep(1000);
					if (FALSE == DUT_CONNECT && TRUE == myPing(CON_DUTIP)) // ���֮ǰ�Ǵ���û�����ӵ�״̬
					{
						g_Dlg->updateResult("����ȡ�豸��Ϣ", RGB(0,0,0));

						// ͨ��web�����ȡ��Ʒ��Ϣ��ʾ����������
						int ret = getDevInfo();
						g_Dlg->updateDevicInfo();
						if (ret == TRUE)
						{
							// ����LTE�豸����ȡWiFi Band
							if (CON_DevType == DEVICE_LTE_MIFI || CON_DevType ==
							DEVICE_LTE_MSM8909 || CON_DevType == DEVICE_CARDV)
								g_Dlg->updateWiFiBandInfo();

							DUT_CONNECT = TRUE;

							g_Dlg->updateResult("  �������豸", RGB(0,255,0));
						}
						else
						{
							DUT_CONNECT = FALSE;
							g_Dlg->updateResult("  δ�����豸", RGB(255,0,0));

							CON_HT40 = FALSE;
							CON_ROUTE_CHECK_RESULT = FALSE;  // �����豸ʱ���½������̼���ʼ��
							DEVID_GET = FALSE;
							runState.checkMIC = BEGIN_CHECK_MIC;
						}
					}
				}
				#if 0
				else
				{
					DUT_CONNECT = FALSE; // �޸�DUT����״̬
					g_Dlg->updateResult("δ���ӶԶ�PC", RGB(255,0,0));
					runState.stop = TRUE;
					g_Dlg->strCurState = "";

					// ����豸��Ϣ
					product->initial = 0;
					clearDevInfo();
					g_Dlg->updateDevicInfo();
				}
				#endif

			} else
			{
				DUT_CONNECT = FALSE; // �޸�DUT����״̬
				g_Dlg->updateResult("  δ�����豸", RGB(255,0,0));
				runState.stop = TRUE;
				g_Dlg->strCurState = "";

				CON_HT40 = FALSE;

				CON_ROUTE_CHECK_RESULT = FALSE;  // �����豸ʱ���½������̼���ʼ��
				DEVID_GET = FALSE;
				runState.checkMIC = BEGIN_CHECK_MIC;

				// ����豸��Ϣ
				product->initial = 0;
				clearDevInfo();
				g_Dlg->updateDevicInfo();
			}
		} else if(STOP_PING_DUT == runState.pingDUT) // �޸�ѡ��
		{
			runState.stop = TRUE;
			DUT_CONNECT = FALSE; // �޸�DUT����״̬
			g_Dlg->updateResult("  δѡ���豸", RGB(0,0,0));
			g_Dlg->strCurState = "";
			runState.stop = TRUE;

			CON_HT40 = FALSE;

			CON_ROUTE_CHECK_RESULT = FALSE;  // �����豸ʱ���½������̼���ʼ��
			DEVID_GET = FALSE;
			runState.checkMIC = BEGIN_CHECK_MIC;

			// ����豸��Ϣ
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
  Description: ����MIC�������̣߳����̻߳���ѡ����ͺ�һֱ���м����MIC������������״̬
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
				SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "������MIC");
			}
			else
			{
				MIC_CONNECT = FALSE;
				SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "δ����MIC");
				ReleaseMutex(hMutex_mic);
				continue;
			}

			// ��ȡdevid
			if (CON_MES_ENABLE && TRUE == DUT_CONNECT)
			{
				//g_Dlg->updateResult("����ȡdevid", RGB(0,0,0));
				// TODO
				// ��ȡdevid
				//devid = mic->ObtainDeviceID(CON_LINE, CON_STATION);
				//devid = mic->ObtainDeviceIDwithType(CON_LINE, CON_STATION, NX_OUTDOORAP);
				int devid_get_num = 0;
				while (!DEVID_GET && DUT_CONNECT)
				{
					devid = productWebInfo.product_devid;
					// ���devid
					if (!mic->CheckDevID(devid))
					{
						DEVID_GET = FALSE;
						devid_get_num++;
						if (devid_get_num >= DEVID_GET_NUM)
						{
							//g_Dlg->updateCurState(">>��ȡDEVIDʧ��");
							g_Dlg->updateResult("  δ��ȡDEVID", RGB(255,0,0));
							runState.checkMIC = CHECK_MIC_FAIL;
							break;
							//runState.pingDUT = PAUSE_PING_DUT;
						}
					}
					else
					{
						DEVID_GET = TRUE;
						g_Dlg->updateCurState(">>��ȡDEVID���");
					}
				}

				if (CON_ROUTE_CHECK_ENABLE && !CON_ROUTE_CHECK_RESULT)
				{
					//int len;
					//char **mac = mic->GetDevidByMac("0000000011D0", &len);
					//g_Dlg->updateResult("�����̼��", RGB(0,0,0));
					int route_check_num = 0;
					while(!CON_ROUTE_CHECK_RESULT && DUT_CONNECT && CHECK_MIC_FAIL != runState.checkMIC)
					{
						int route_check_result = mic->RoutingCheck("T40", CON_CHECK_STATION, devid, "");
						switch (route_check_result)
						{
						case OK:
							//SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "���̼��ͨ��");
							g_Dlg->updateCurState(">>���̼�����");
							CON_ROUTE_CHECK_RESULT= TRUE;
							runState.checkMIC = CHECK_MIC_SUCCESS;
							break;
						case ROUTING_CHECK_FAIL:
						case ROUTING_CHECK_NO_TEST:
						default:
							//SetDlgItemText(g_Dlg->hWnd, IDC_MIC, "δ�鵽��һվ��������");
							CON_ROUTE_CHECK_RESULT = FALSE;
							route_check_num++;
							if (route_check_num >= ROUTE_CHECK_NUM)
							{
								//g_Dlg->updateCurState(">>���̼��δͨ��");
								g_Dlg->updateResult("  δͨ�����", RGB(255,0,0));
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
					g_Dlg->updateCurState(">>�������̼��");
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
			else if (DUT_STATE_TESTED == DUTState && mic->ConnectCheck() && mic->IsBufMode)  // ����������Ѳ�����
			{
				mic->BufferDataUpload("T40", &length);
				mic->IsBufMode = FALSE;
				g_Dlg->updateCurState(">>�����������ϴ�");
			}
#endif
			ReleaseMutex(hMutex_mic);
		}

		/* When mic is not ok, test is not allowed */
		g_Dlg->GetDlgItemText(IDC_TESTTHROUGHPUT, strButton);
		if (strButton == _T("��ʼ����"))
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

	//  MIC��Դ�ͷ�
	mic->Release();
	delete[] mic;
	return TRUE;
}

/*******************************************************************************
  Function:    mac_compare
  Description: �Լ������mac�ȽϷ���
  Author:      liguoliang
  Date:        2014-09-05
*******************************************************************************/
int mac_compare(char* mac1, char* mac2)
{
	int ret = 0;

	char format_str1[13] = {0};
	char format_str2[13] = {0};

	int i,j;

	// ���Դ�Сд�Լ�ת���ɹ̶���ʽ
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
	// �ϴ�����
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
			setField(idn.Failmsg, sizeof(idn.Failmsg), "����δͨ��");
		}
		else
		{
			setField(idn.Failmsg, sizeof(idn.Failmsg), "δ��������������");
		}
		idn.Failcode = T40_FAIL_PERF;
	}
	else
	{
		setField(idn.Result, sizeof(idn.Result), "P");

		if (!CON_ROUTE_CHECK_ENABLE)
		{
			setField(idn.Failmsg, sizeof(idn.Failmsg), "δ�������̼��");
		}

		idn.Failcode = T40_PASS;
	}

	//setField(idn.Deviceid, sizeof(idn.Deviceid), devid);
	if (strlen(productDBInfo.product_mac) == mac_length)
	{
		setField(idn.Macid, sizeof(idn.Macid), productDBInfo.product_mac);
	}
	setField(idn.Hardwareid, sizeof(idn.Hardwareid), "");  //  ���ֶ�
	setField(idn.Line, sizeof(idn.Line), CON_LINE);
	setField(idn.Workstation, sizeof(idn.Workstation), station_number);
	setField(idn.Userno, sizeof(idn.Userno), nb);
	setField(idn.Equipmentno, sizeof(idn.Equipmentno), CON_EQUIP);
	setField(idn.Starttime, sizeof(idn.Starttime), starttime);
	setField(idn.Endtime, sizeof(idn.Endtime), endtime);
	setField(idn.Ver, sizeof(idn.Ver), title);  //  ����汾

	// �������ݸ�ֵ
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

	// ���ܲ�������
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
	// �����ж�
	bool connected = mic->ConnectCheck();
	if (connected)
	{
		//  �����ϴ�����Ϊֱ��ģʽ
		mic->IsBufMode = false;
	}
	else
	{
		//  �����ϴ�����Ϊ����ģʽ
		mic->IsBufMode = true;
		g_Dlg->updateCurState(">>�޷����ӵ�MIC");
	}
#endif

	int rst = mic->DataUpload(idn, paras, 1);
	if (rst < 0)
	{
		g_Dlg->updateCurState(">>���ϴ������ػ���");
	}
	else
	{
		g_Dlg->updateCurState(">>���ϴ���MIC������");
	}

#if 0
	if (mic->IsBufMode)
	{
		g_Dlg->updateCurState(">>���ϴ������ػ���");
	}
	else
	{
		g_Dlg->updateCurState(">>���ϴ���MIC������");
	}

	// ���ʵ���ʱ���ش����������
	mic->BufferDataUpload("T40", &length);
#endif
    return rst;
}

