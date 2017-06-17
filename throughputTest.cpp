/*********************************************************
* Copyright (c) TP-LINK Technologies CO., LTD.
* All rights reserved.
*文 件 名：   ThroughputTest.c
*摘    要：   测试DUT吞吐量的几个函数
*作    者：   邓国伟
*创建日期：   2007-10-13
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

int opencmd=0x11;     //开屏蔽箱指令
int closecmd=0x22;      //关屏蔽箱指令
char opencmdnew[]="open\r\n";     //开屏蔽箱指令
char closecmdnew[]="close\r\n";      //关屏蔽箱指令

extern CMyDlg  *g_Dlg;

void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING what);

/**********************************************************************
*函 数 名：   beginThroughputTest
*摘    要：   测试DUT吞吐量，若在测试过程中runState.stop == TRUE，
*             则停止测试
*输    入：   e1 - 本地IP地址
*             e2 - 对端的IP地址
*             script - 测试执行的脚本文件名
*             timeOut - 测试的超时时间，若超过该时间测试未结束则强行关闭
                        测试并获得结果
*返 回 值：   AVG - 平均吞吐量
*作    者：   邓国伟
*创建日期：   2007-10-13
**************************************************************8*********/
CHR_FLOAT beginThroughputTest()
{
	//测试句柄
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

	//储存错误信息
	char errorInfo[CHR_MAX_ERROR_INFO];

	//Chariot API 的返回值
	CHR_API_RC rc;

	runState.stop = FALSE;

	//初始化
	rc = CHR_api_initialize(CHR_DETAIL_LEVEL_ALL, errorInfo, CHR_MAX_ERROR_INFO, &len);
	if (CHR_OK != rc)
	{
		//初始化失败，不能得到失败代码，所以不能调用show_error()。显示某些知道的信息
		str.Format("初始化IxChariot的API失败！\n失败代码：%d\n失败信息：%s",
			rc, errorInfo);
		AfxMessageBox(str);
		return FALSE;
	}

	//创建新的test对象或加载一个已经存在的
	if (CHR_OK == rc)
	{
		//printf("Create the test...\n");
		rc = CHR_test_new(&test);
		if (rc != CHR_OK)
		{
			show_error((CHR_HANDLE)NULL, rc, "创建新的测试失败");
			return FALSE;
		}
	}

	if (CHR_OK == rc)
	{
		rc = CHR_test_load(test, CON_testFilename, strlen(CON_testFilename));
		if (rc != CHR_OK)
		{
			show_error(test, rc, "加载测试文件失败");
			return FALSE;
		}
	}

	//获取pair数目
	if (CHR_OK == rc)
	{
		rc = CHR_test_get_pair_count(test, &pairCount);
		if (CHR_OK != rc)
		{
			show_error(test, rc, "取不到测试pair的数目");
			goto release;
		}
	}

	if (CHR_OK == rc)
	{
		pair = (CHR_PAIR_HANDLE *) malloc(sizeof(CHR_PAIR_HANDLE) * pairCount);
		if (NULL == pair)
			show_error(NULL, GetLastError(), "给pair分配内存失败");
		metime = (CHR_FLOAT *) malloc(sizeof(CHR_FLOAT) * pairCount);
		if (NULL == metime)
			show_error(NULL, GetLastError(), "给metime分配内存失败");
	}

	//获得pair的句柄
	if (CHR_OK == rc)
	{
		for (int i = 0; i < pairCount; i++)
		{
			rc = CHR_test_get_pair(test, i, &pair[i]);
			if (CHR_OK != rc)
			{
				show_error(test, rc, "获取测试pair的句柄失败");
				goto release;
			}
		}
	}

	//运行test
	if (CHR_OK == rc)
	{
		//printf("Run the test...\n");
		rc = CHR_test_start(test);
		if (rc != CHR_OK)
		{
			show_error(test, rc, "运行测试失败");
			goto release;
		}
	}

	//等待测试结束并输出运行状态
	CHR_COUNT begin, end1, end2;
	begin = end1 = end2 = clock();
	for (i = 0; ; i++)
	{
		rc = CHR_test_query_stop(test, 1);  //等待stop完成
		if (rc == CHR_TIMED_OUT)
			CHR_pair_get_runStatus(pair[0], &runStatus);
		if ((1 == runStatus) || (2 == runStatus) || (3 == runStatus))
		{
			end1 = clock();
			str.Format("正在初始化：已过 %d 秒", (end1 - begin)/1000);
		}
		if (5 == runStatus)
		{
			end2 = clock();
			str.Format("正在运行：已过 %d 秒", (end2 - end1)/1000);
		}
		m_Avg->SetWindowText(str);


		if (runState.stop == TRUE)
		{
			rc = CHR_test_stop(test); //stop命令
			rc = CHR_test_query_stop(test, 3);  //等待stop完成
			if ((rc != CHR_OK)/* && (rc != CHR_TIMED_OUT)*/)//**这句代码有问题，再想想
			{
				rc = CHR_test_abandon(test);
				if (rc != CHR_OK)
				{
					show_error(test, rc, "放弃测试失败");
				}
			}
			str.Format("已停止运行");
			m_Avg->SetWindowText(str);
			goto release;
		}
		if (rc == CHR_OK)
		{
			if ((1 == runStatus) || (2 == runStatus) || (3 == runStatus))
			{
				show_error(NULL, rc, "测试初始化失败");
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
				show_error(test, rc, "放弃测试失败");
			}
		}
	}

	//取得测试结果
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

	//计算测试结果
	for (i = 0; i < pairCount; i++)
	{
		longestElapse = (longestElapse > metime[i])? longestElapse:metime[i];
	}
	if (0 == longestElapse)
		groupAvg = 0;
	else
		groupAvg = groupAvg * 8 / M /longestElapse;

release:
	/* 保存测试文件，以便于日后查看 */
	rc = CHR_test_save(test);
    if (rc != CHR_OK)
	{
      show_error(test, rc, "保存测试失败");
    }


	//释放未释放资源
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
*函 数 名：   show_error
*摘    要：   显示错误信息
*输    入：   handle - 错误发生的对象句柄。若为NULL表示错误不是
*                      发生在调用IxChariot的时候，若不为NULL，表示
*                      错误发生在调用IxChariot的API的时候，当code为-1
*                      表示没有错误代码。
*             code -  返回的错误码
*             what - 错误调用发生的函数名
*作    者：   邓国伟
*创建日期：   2007-10-13
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

	if (NULL == handle) //表示非IxChariot调用失败
	{
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s,错误代码为 %d",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code);

		AfxMessageBox(str);
		str.Insert(100, "\r\n\r\n===========\r\n===========\r\n");
		fprintf(fp, str);
		fclose(fp);

		return;
	}

	//以下表示是IxChariot调用失败

	//得到返回值对应的信息
	rc = CHR_api_get_return_msg(code, msg, CHR_MAX_RETURN_MSG, &msgLen);
	if (rc != CHR_OK)
	{
		//得不到消息，显示为什么
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s！\r\n不能得到错误代码为%d－%d的错误信息。\r\n",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code, rc);

		fprintf(fp, str);
		AfxMessageBox(str);
	}
	else
	{
		//告诉用户错误信息
		str.Format("%d-%d-%d  %d:%d:%d  \r\n%s！\r\n错误代码：%d(%s)\r\n",
			systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			what, code, msg);
		fprintf(fp, str);
		AfxMessageBox(str);
	}

	/* 看是否还有更多可用的错误信息。这只对返回值是CHR_OPERATION_FAILED和CHR_OBJECT_INVALID
	 * 是有用的。当"new" function calls失败时，因为没有返回handle，所以不能检查更多的错误信息
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
			// 不能忽略所有不成功的返回值，同时NO_SUCH_VALUE返回值意味着没有可用的信息
			str.Format("更多错误信息：%s\r\n===========\r\n===========\r\n", errorInfo);
			fprintf(fp, str);
			//AfxMessageBox(str);
		}
	}
	fclose(fp);
}

/*********************************************************
*函 数 名：   pingRequest
*摘    要：   查找和对方主机是否已经连通，已连通返回TRUE；
*             否则，每隔一秒钟查看一次是否已经连通，除非stop为TRUE。
*作    者：   邓国伟
*创建日期：   2007-10-31
*修改历史：   2013-12-01
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
			str.Format("正在连接DUT，已过 %d 秒", (end - begin)/1000);
		else
			str.Format("正在和%s建立连接，已过 %d 秒", IPaddr, (end - begin)/1000);
		m_Avg->SetWindowText(str);


		// [liguoliang start] 修改ping的方式是通过调用dll中的函数来完成
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
*函 数 名：   pingRequestAgain
*摘    要：   等待对方重启。
*             和对方连续三次ping通则认为已连通，已连通返回TRUE；
*             否则，每隔一秒钟查看一次是否已经连通，除非stop为TRUE。
*作    者：   邓国伟
*创建日期：   2007-10-31
*修改历史：   2013-12-01
*********************************************************/
bool pingRequestAgain(char IPaddr[16])
{
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);
	CString str;
	int begin, end;
	int iRet;

	int passCount = 0;

	//连续3次ping不通则认为其已经断开
	begin = clock();
	while(passCount < 3)
	{
		end = clock();

		// [liguoliang start] 修改ping的方式是通过调用dll中的函数来完成
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
	//连续3次ping通则认为其已经连接
	begin = clock();
	while(passCount < 3)
	{
		end = clock();
		str.Format("正在重新连接DUT，已过 %d 秒", (end - begin)/1000);
		m_Avg->SetWindowText(str);

		// [liguoliang start] 修改ping的方式是通过调用dll中的函数来完成
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
  Description: 调用icmp.dll的函数进行ping的操作,以替代原来的调用ping命令并解析文件的方式
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
*函 数 名：   closeShield
*摘    要：   关闭屏蔽箱操作
*作    者：   hujia
*创建日期：   2010-03-22
*********************************************************/
DWORD WINAPI closeShield(void)
{

	int bResult;
	unsigned long bwrite;
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //清缓冲区
	bResult=WriteFile(hCom,&closecmd,1,&bwrite,NULL);                         //串口发数据(老屏蔽箱)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	Sleep(100);
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //清缓冲区
	bResult=WriteFile(hCom,&closecmdnew,strlen(closecmdnew),&bwrite,NULL);    //串口发数据(新屏蔽箱)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	return TRUE;
}

/*********************************************************
*函 数 名：   openShield
*摘    要：   开启屏蔽箱操作
*作    者：   hujia
*创建日期：   2010-03-22
*********************************************************/
DWORD WINAPI openShield(void)
{

	int bResult;
	unsigned long bwrite;
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);  //清缓冲区
	bResult=WriteFile(hCom,&opencmd,1,&bwrite,NULL);                          //串口发数据(老屏蔽箱)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}

	Sleep(100);
	PurgeComm(hCom,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);   //清缓冲区
	bResult=WriteFile(hCom,opencmdnew, strlen(opencmdnew),&bwrite,NULL);       //串口发数据(新屏蔽箱)
	if (!bResult)
	{
		printf("write err!");
		return 0;
	}
	return TRUE;
}

/*********************************************************
*函 数 名：   waitDUTStartup
*摘    要：   等待DUT启动，若能和DUT连接上，则认为DUT已启动。
*作    者：   邓国伟
*创建日期：   2007-10-15
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
*函 数 名：   waitDUTReStartup
*摘    要：   等待DUT重新启动，首先应该有一次连接不上，再接着连接上
*作    者：   邓国伟
*创建日期：   2007-10-17
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

	//没连接上，表示DUT关闭
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

	//与DUT连接上即表示DUT已启动
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
