/*********************************************************
* Copyright (c) TP-LINK Technologies CO., LTD.
* All rights reserved.
*�� �� ����   ThroughputTest.c
*ժ    Ҫ��   ����DUT�������ļ�������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-13
*********************************************************/
#include "ThroughputTest.h"
#include "stdafx.h"
#include "WirelessTestDlg.h"
#include "chrapi.h"
#include "configure.h"

// [liguoliang start]
#include "icmp.h"
// [liguoliang end]

#define ERRORINFOFILENAME      ".//log//errorInfo.log"

int opencmd=0x11;     //��������ָ��
int closecmd=0x22;      //��������ָ��
char opencmdnew[]="open\r\n";     //��������ָ��
char closecmdnew[]="close\r\n";      //��������ָ��

extern CMyDlg  *g_Dlg;

void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING what);

/**********************************************************************
*�� �� ����   beginThroughputTest
*ժ    Ҫ��   ����DUT�����������ڲ��Թ�����runState.stop == TRUE��
*             ��ֹͣ����
*��    �룺   e1 - ����IP��ַ
*             e2 - �Զ˵�IP��ַ
*             script - ����ִ�еĽű��ļ���
*             timeOut - ���Եĳ�ʱʱ�䣬��������ʱ�����δ������ǿ�йر�
                        ���Բ���ý��
*�� �� ֵ��   AVG - ƽ��������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-13
**************************************************************8*********/
CHR_FLOAT beginThroughputTest()
{
	//���Ծ��
	CHR_TEST_HANDLE test = (CHR_TEST_HANDLE)NULL;
	CHR_PAIR_HANDLE *pair = NULL;
	CHR_FLOAT *metime = NULL;
	CHR_TIMINGREC_HANDLE trecord;

	CHR_COUNT pairCount = 0;
	CHR_FLOAT groupAvg = 0.0;
	CHR_FLOAT bytes = 0.0;
	CHR_FLOAT M = 1000000.0;  //1M = 1000000bytes
	CHR_FLOAT longestElapse = 0.0;

	int i = 0;


	CHR_LENGTH len;
	CString str;

	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);
	CHR_PAIR_RUNSTATUS_TYPE runStatus;

	//���������Ϣ
	char errorInfo[CHR_MAX_ERROR_INFO];

	//Chariot API �ķ���ֵ
	CHR_API_RC rc;

	runState.stop = FALSE;

	//��ʼ��
	rc = CHR_api_initialize(CHR_DETAIL_LEVEL_ALL, errorInfo, CHR_MAX_ERROR_INFO, &len);
	if (CHR_OK != rc)
	{
		//��ʼ��ʧ�ܣ����ܵõ�ʧ�ܴ��룬���Բ��ܵ���show_error()����ʾĳЩ֪������Ϣ
		str.Format("��ʼ��IxChariot��APIʧ�ܣ�\nʧ�ܴ��룺%d\nʧ����Ϣ��%s",
			rc, errorInfo);
		AfxMessageBox(str);
		return FALSE;
	}

	//�����µ�test��������һ���Ѿ����ڵ�
	if (CHR_OK == rc)
	{
		//printf("Create the test...\n");
		rc = CHR_test_new(&test);
		if (rc != CHR_OK)
		{
			show_error((CHR_HANDLE)NULL, rc, "�����µĲ���ʧ��");
			return FALSE;
		}
	}

	if (CHR_OK == rc)
	{
		rc = CHR_test_load(test, CON_testFilename, strlen(CON_testFilename));
		if (rc != CHR_OK)
		{
			show_error(test, rc, "���ز����ļ�ʧ��");
			return FALSE;
		}
	}

	//��ȡpair��Ŀ
	if (CHR_OK == rc)
	{
		rc = CHR_test_get_pair_count(test, &pairCount);
		if (CHR_OK != rc)
		{
			show_error(test, rc, "ȡ��������pair����Ŀ");
			goto release;
		}
	}

	if (CHR_OK == rc)
	{
		pair = (CHR_PAIR_HANDLE *) malloc(sizeof(CHR_PAIR_HANDLE) * pairCount);
		if (NULL == pair)
			show_error(NULL, GetLastError(), "��pair�����ڴ�ʧ��");
		metime = (CHR_FLOAT *) malloc(sizeof(CHR_FLOAT) * pairCount);
		if (NULL == metime)
			show_error(NULL, GetLastError(), "��metime�����ڴ�ʧ��");
	}

	//���pair�ľ��
	if (CHR_OK == rc)
	{
		for (int i = 0; i < pairCount; i++)
		{
			rc = CHR_test_get_pair(test, i, &pair[i]);
			if (CHR_OK != rc)
			{
				show_error(test, rc, "��ȡ����pair�ľ��ʧ��");
				goto release;
			}
		}
	}

	//����test
	if (CHR_OK == rc)
	{
		//printf("Run the test...\n");
		rc = CHR_test_start(test);
		if (rc != CHR_OK)
		{
			show_error(test, rc, "���в���ʧ��");
			goto release;
		}
	}

	//�ȴ����Խ������������״̬
	CHR_COUNT begin, end1, end2;
	begin = end1 = end2 = clock();
	for (i = 0; ; i++)
	{
		rc = CHR_test_query_stop(test, 1);  //�ȴ�stop���
		if (rc == CHR_TIMED_OUT)
			CHR_pair_get_runStatus(pair[0], &runStatus);
		if ((1 == runStatus) || (2 == runStatus) || (3 == runStatus))
		{
			end1 = clock();
			str.Format("���ڳ�ʼ�����ѹ� %d ��", (end1 - begin)/1000);
		}
		if (5 == runStatus)
		{
			end2 = clock();
			str.Format("�������У��ѹ� %d ��", (end2 - end1)/1000);
		}
		m_Avg->SetWindowText(str);


		if (runState.stop == TRUE)
		{
			rc = CHR_test_stop(test); //stop����
			rc = CHR_test_query_stop(test, 3);  //�ȴ�stop���
			if ((rc != CHR_OK)/* && (rc != CHR_TIMED_OUT)*/)//**�����������⣬������
			{
				rc = CHR_test_abandon(test);
				if (rc != CHR_OK)
				{
					show_error(test, rc, "��������ʧ��");
				}
			}
			str.Format("��ֹͣ����");
			m_Avg->SetWindowText(str);
			goto release;
		}
		if (rc == CHR_OK)
		{
			if ((1 == runStatus) || (2 == runStatus) || (3 == runStatus))
			{
				show_error(NULL, rc, "���Գ�ʼ��ʧ��");
				goto release;
			}
			break;
		}
		else if (rc == CHR_TIMED_OUT)
		{
			//void
		}
		else
		{
			rc = CHR_test_abandon(test);
			if (rc != CHR_OK)
			{
				show_error(test, rc, "��������ʧ��");
			}
		}
	}

	//ȡ�ò��Խ��
	if (CHR_OK == rc)
	{
		for (i = 0; i < pairCount; i++)
		{
			bytes = 0;
			rc = CHR_common_results_get_bytes_recv_e1(pair[i], &bytes);
			groupAvg += bytes;
			bytes = 0;
			rc = CHR_common_results_get_bytes_sent_e1(pair[i], &bytes);
			groupAvg += bytes;

			metime[i] = 0;
			CHR_COUNT num;
			CHR_pair_get_timing_record_count(pair[i], &num);
			CHR_pair_get_timing_record(pair[i], num-1, &trecord);
			CHR_timingrec_get_elapsed(trecord, &metime[i]);
		}
	}

	//������Խ��
	for (i = 0; i < pairCount; i++)
	{
		longestElapse = (longestElapse > metime[i])? longestElapse:metime[i];
	}
	if (0 == longestElapse)
		groupAvg = 0;
	else
		groupAvg = groupAvg * 8 / M /longestElapse;

release:
	/* ��������ļ����Ա����պ�鿴 */
	rc = CHR_test_save(test);
    if (rc != CHR_OK)
	{
      show_error(test, rc, "�������ʧ��");
    }


	//�ͷ�δ�ͷ���Դ
	for (i = 0; i < pairCount; i++)
	{
		if ( pair[i] != (CHR_PAIR_HANDLE)NULL )
		{
			CHR_pair_delete(pair[i]);
		}
	}
	if ( test != (CHR_TEST_HANDLE)NULL )
	{
		CHR_test_delete(test);
		test = (CHR_TEST_HANDLE)NULL;
	}
	if (pair != NULL)
	{
		free(pair);
		pair = NULL;
	}
	if (metime != NULL)
	{
		free(metime);
		metime = NULL;
	}

	return groupAvg;
}


/*********************************************************
*�� �� ����   show_error
*ժ    Ҫ��   ��ʾ������Ϣ
*��    �룺   handle - �������Ķ���������ΪNULL��ʾ������
*                      �����ڵ���IxChariot��ʱ������ΪNULL����ʾ
*                      �������ڵ���IxChariot��API��ʱ�򣬵�codeΪ-1
*                      ��ʾû�д�����롣
*             code -  ���صĴ�����
*             what - ������÷����ĺ�����
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-13
*********************************************************/
void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING what)
{
	char msg[CHR_MAX_RETURN_MSG];
	CHR_LENGTH msgLen;
	char errorInfo[CHR_MAX_ERROR_INFO];
	CHR_LENGTH errorLen;
	CHR_API_RC rc;
	CString str;

	FILE *fp;
	fp = fopen(ERRORINFOFILENAME, "a");

	SYSTEMTIME systime;
	GetLocalTime(&systime);

	if (NULL == handle) //��ʾ��IxChariot����ʧ��
	{
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s,�������Ϊ %d",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code);

		AfxMessageBox(str);
		str.Insert(100, "\r\n\r\n===========\r\n===========\r\n");
		fprintf(fp, str);
		fclose(fp);

		return;
	}

	//���±�ʾ��IxChariot����ʧ��

	//�õ�����ֵ��Ӧ����Ϣ
	rc = CHR_api_get_return_msg(code, msg, CHR_MAX_RETURN_MSG, &msgLen);
	if (rc != CHR_OK)
	{
		//�ò�����Ϣ����ʾΪʲô
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s��\r\n���ܵõ��������Ϊ%d��%d�Ĵ�����Ϣ��\r\n",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code, rc);

		fprintf(fp, str);
		AfxMessageBox(str);
	}
	else
	{
		//�����û�������Ϣ
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s��\r\n������룺%d(%s)\r\n",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code, msg);
		fprintf(fp, str);
		AfxMessageBox(str);
	}

	/* ���Ƿ��и�����õĴ�����Ϣ����ֻ�Է���ֵ��CHR_OPERATION_FAILED��CHR_OBJECT_INVALID
	 * �����õġ���"new" function callsʧ��ʱ����Ϊû�з���handle�����Բ��ܼ�����Ĵ�����Ϣ
	 */
	if ((code == CHR_OPERATION_FAILED
		|| code == CHR_OBJECT_INVALID)
		&& handle != (CHR_HANDLE)NULL)
	{
		rc = CHR_common_error_get_info(handle,
			CHR_DETAIL_LEVEL_ALL,
			errorInfo,
			CHR_MAX_ERROR_INFO,
			&errorLen);
		if (rc == CHR_OK)
		{
			// ���ܺ������в��ɹ��ķ���ֵ��ͬʱNO_SUCH_VALUE����ֵ��ζ��û�п��õ���Ϣ
			str.Format("���������Ϣ��%s\r\n===========\r\n===========\r\n", errorInfo);
			fprintf(fp, str);
			//AfxMessageBox(str);
		}
	}
	fclose(fp);
}

/*********************************************************
*�� �� ����   pingRequest
*ժ    Ҫ��   ���ҺͶԷ������Ƿ��Ѿ���ͨ������ͨ����TRUE��
*             ����ÿ��һ���Ӳ鿴һ���Ƿ��Ѿ���ͨ������stopΪTRUE��
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-31
*�޸���ʷ��   2013-12-01
*********************************************************/
 bool pingRequest(char IPaddr[16])
{
	CString str;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);
	int begin, end;

	begin = clock();
	while(1)
	{
		end = clock();
		if (0 == strcmp(IPaddr, CON_DUTIP))
			str.Format("��������DUT���ѹ� %d ��", (end - begin)/1000);
		else
			str.Format("���ں�%s�������ӣ��ѹ� %d ��", IPaddr, (end - begin)/1000);
		m_Avg->SetWindowText(str);


		// [liguoliang start] �޸�ping�ķ�ʽ��ͨ������dll�еĺ��������
		if (icmpDll != NULL)
		{
			if (TRUE == runState.stop)
			{
				m_Avg->SetWindowText("");
				return FALSE;
			}

			if (myPing(IPaddr) == TRUE)
			{
				m_Avg->SetWindowText("");
				return TRUE;
			}

			/* if wait time is large than 10s, exit */
			if ((end - begin)/1000 > 10)
			{
				m_Avg->SetWindowText("");
				return FALSE;
			}
		} else
		{
			return FALSE;
		}
		// [liguoliang end]

		_sleep(500);
	}
}

/*********************************************************
*�� �� ����   pingRequestAgain
*ժ    Ҫ��   �ȴ��Է�������
*             �ͶԷ���������pingͨ����Ϊ����ͨ������ͨ����TRUE��
*             ����ÿ��һ���Ӳ鿴һ���Ƿ��Ѿ���ͨ������stopΪTRUE��
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-31
*�޸���ʷ��   2013-12-01
*********************************************************/
bool pingRequestAgain(char IPaddr[16])
{
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);
	CString str;
	int begin, end;
	int iRet;

	int passCount = 0;

	//����3��ping��ͨ����Ϊ���Ѿ��Ͽ�
	begin = clock();
	while(passCount < 3)
	{
		end = clock();

		// [liguoliang start] �޸�ping�ķ�ʽ��ͨ������dll�еĺ��������
		if (icmpDll != NULL)
		{
			if (TRUE == runState.stop)
			{
				m_Avg->SetWindowText("");
				return FALSE;
			}

			if (myPing(IPaddr) == TRUE)
			{
				passCount = 0;
				break;
			} else
			{
				passCount++;
			}
		} else
	    {
			return FALSE;
		}
		// [liguoliang end]

		_sleep(500);
	}

	passCount = 0;
	//����3��pingͨ����Ϊ���Ѿ�����
	begin = clock();
	while(passCount < 3)
	{
		end = clock();
		str.Format("������������DUT���ѹ� %d ��", (end - begin)/1000);
		m_Avg->SetWindowText(str);

		// [liguoliang start] �޸�ping�ķ�ʽ��ͨ������dll�еĺ��������
		if (icmpDll != NULL)
		{
			if (TRUE == runState.stop)
			{
				m_Avg->SetWindowText("");
				return FALSE;
			}

			if (myPing(IPaddr) == TRUE)
			{
				passCount = 0;
				break;
			} else
			{
				passCount++;
			}
		} else
		{
			return FALSE;
		}
		// [liguoliang end]

		_sleep(500);

	}

	m_Avg->SetWindowText("");
	return TRUE;
}

/*******************************************************************************
  Function:    myPing
  Description: ����icmp.dll�ĺ�������ping�Ĳ���,�����ԭ���ĵ���ping��������ļ��ķ�ʽ
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
int myPing(char IPAddr[16])
{
	HANDLE hIcmp;
	char *SendData = "ICMP SEND DATA";
	LPVOID ReplyBuffer;
	DWORD dwRetVal;
	DWORD buflen;
	PIP_ECHO_REPLY pIcmpEchoReply;
	int result = 0;

	hIcmp = pIcmpCreateFile();

	buflen = sizeof(IP_ECHO_REPLY) + strlen(SendData) + 1;
	ReplyBuffer = (VOID*) malloc(buflen);

	if (ReplyBuffer == NULL) {
		return 1;
	}
	memset(ReplyBuffer, 0, buflen);
	pIcmpEchoReply = (PIP_ECHO_REPLY)ReplyBuffer;

	dwRetVal = pIcmpSendEcho(hIcmp,
		inet_addr(IPAddr),
		SendData, strlen(SendData),
		NULL, ReplyBuffer,
		buflen,
		1000);
	if (dwRetVal != 0) {
		result = TRUE;
	} else {
		result = FALSE;
	}

	pIcmpCloseHandle(hIcmp);
	free(ReplyBuffer);

	return result;
}

/*********************************************************
*�� �� ����   closeShield
*ժ    Ҫ��   �ر����������
*��    �ߣ�   hujia
*�������ڣ�   2010-03-22
*********************************************************/
DWORD WINAPI closeShield(void)
{

	int bResult;
	unsigned long bwrite;
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //�建����
	bResult=WriteFile(hCom,&closecmd,1,&bwrite,NULL);                         //���ڷ�����(��������)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	Sleep(100);
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //�建����
	bResult=WriteFile(hCom,&closecmdnew,strlen(closecmdnew),&bwrite,NULL);    //���ڷ�����(��������)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	return TRUE;
}

/*********************************************************
*�� �� ����   openShield
*ժ    Ҫ��   �������������
*��    �ߣ�   hujia
*�������ڣ�   2010-03-22
*********************************************************/
DWORD WINAPI openShield(void)
{

	int bResult;
	unsigned long bwrite;
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //�建����
	bResult=WriteFile(hCom,&opencmd,1,&bwrite,NULL);                          //���ڷ�����(��������)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}

	Sleep(100);
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);   //�建����
	bResult=WriteFile(hCom,opencmdnew, strlen(opencmdnew),&bwrite,NULL);       //���ڷ�����(��������)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	return TRUE;
}

/*********************************************************
*�� �� ����   waitDUTStartup
*ժ    Ҫ��   �ȴ�DUT���������ܺ�DUT�����ϣ�����ΪDUT��������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-15
*********************************************************/
bool waitDUTStartup()
{
	SOCKET sock;
	struct sockaddr_in DUT_addr;

	WinExec("arp -d", SW_HIDE);
	if ((TRUE == runState.pause) || (TRUE == runState.stop))
	{
		//runState.pause = FALSE;
		//runState.stop  = FALSE;
		return FALSE;
	}

	DUT_addr.sin_family = AF_INET;
	DUT_addr.sin_port = htons(80);
	DUT_addr.sin_addr.S_un.S_addr = inet_addr(CON_DUTIP);
	memset(DUT_addr.sin_zero, 0, 8);

	fd_set con;
	unsigned long ul = 1;
	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	while (1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		FD_ZERO(&con);
		FD_SET(sock, &con);


		ioctlsocket(sock, FIONBIO, &ul);

		if  (TRUE == runState.stop)
		{
			shutdown(sock, 2);
			closesocket(sock);
			return FALSE;
		}
		connect(sock, (struct sockaddr*)(&DUT_addr), sizeof(struct sockaddr));
		if (select(0, 0, &con, 0, &tv) > 0)
		{
			break;
		}
		shutdown(sock, 2);
		closesocket(sock);
	}
	shutdown(sock, 2);
	closesocket(sock);

	return TRUE;
}

/*********************************************************
*�� �� ����   waitDUTReStartup
*ժ    Ҫ��   �ȴ�DUT��������������Ӧ����һ�����Ӳ��ϣ��ٽ���������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-17
*********************************************************/
bool waitDUTReStartup()
{
	SOCKET sock;
	struct sockaddr_in DUT_addr;
	int clock_begin = 0, clock_end = 0;

	WinExec("arp -d", SW_HIDE);

	DUT_addr.sin_family = AF_INET;
	DUT_addr.sin_port = htons(80);
	DUT_addr.sin_addr.S_un.S_addr = inet_addr(CON_DUTIP);
	memset(DUT_addr.sin_zero, 0, 8);

	fd_set con;

	unsigned long ul = 1;

	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	//û�����ϣ���ʾDUT�ر�
	while(1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		FD_ZERO(&con);
		FD_SET(sock, &con);
		ioctlsocket(sock, FIONBIO, &ul);
		if (TRUE == runState.stop)
		{
			shutdown(sock, 2);
			closesocket(sock);
			return FALSE;
		}
		connect(sock, (struct sockaddr*)(&DUT_addr), sizeof(struct sockaddr));
		if (select(0, 0, &con, 0, &tv) == 0)
		{
			shutdown(sock, 2);
			closesocket(sock);
			_sleep(100);
			break;
		}
		shutdown(sock, 2);
		closesocket(sock);
		_sleep(100);
	}

	_sleep(2000);

	//��DUT�����ϼ���ʾDUT������
	while (1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		FD_ZERO(&con);
		FD_SET(sock, &con);
		ioctlsocket(sock, FIONBIO, &ul);
		if  (TRUE == runState.stop)
		{
			shutdown(sock, 2);
			closesocket(sock);
			_sleep(100);
			return FALSE;
		}
		connect(sock, (struct sockaddr*)(&DUT_addr), sizeof(struct sockaddr));

		if (select(0, 0, &con, 0, &tv) > 0)
		{
			shutdown(sock, 2);
			closesocket(sock);
			_sleep(100);
			break;
		}
		shutdown(sock, 2);
		closesocket(sock);
		_sleep(100);
	}

	return TRUE;
}
