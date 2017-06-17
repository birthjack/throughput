#include "StdAfx.h"
#include "httpDeviceInfo.h"
#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include <locale.h>
#include <atlconv.h>

#include "YCodec.h"
#include "atlbase.h"

#include <stdlib.h>
#include "md5.h"
#include <string.h>

#include <afxinet.h>
#include <WinInet.h>

#include "icmp.h"
#include "configure.h"
#include "WirelessTestDlg.h"


#define MAXBUFFSIZE	4096

//#define UNICODELENGTH 4
#define HEXLENGTH 16
#define MAXPOSTDATASIZE 256

#define MAXDELAYTIME 50

extern CMyDlg  *g_Dlg;

static char sRecvBuf[MAXBUFFSIZE];
static char sRecvBuf_POST[MAXBUFFSIZE];
static char sKeyBuf[][32] = {"devName","sysSoftVer","sysHardVer","macAddr","imei",
							"meid","modemStatus","workingMode", "pinCode", "nonce", "devid"};

extern char *w2c(char *pcstr,const wchar_t *pwstr, size_t len);

/*******************************************************************************
  Function:    sendWebData_PostBand
  Description: 发送配置带宽的请求
  Author:      renjian
  Date:        2015-01-05
*******************************************************************************/
int sendWebData_PostBand(const char *pIP, const CString send_data, int nTimeout)
{
	int sockfd = INVALID_SOCKET,ret = 0;
	struct sockaddr_in serverAddr;
	fd_set t_set;
	struct timeval tv;
	unsigned long lRet;

	int begin;
	int end;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);
	CString str;
	int disconnect_Dut = 0;

	/* 清除缓冲区 */
	memset(sRecvBuf_POST,0 ,sizeof(sRecvBuf_POST));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("创建socket失败 %d\n", WSAGetLastError());
		goto exit;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(80);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(pIP);

	if(SOCKET_ERROR == setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeout,sizeof(nTimeout)))
	{
		perror("Can't set timeout!\n");
		goto exit;
	}

	/* 设置为非阻塞 */
	lRet = 1;
	if (SOCKET_ERROR == ioctlsocket(sockfd, FIONBIO, &lRet))
	{
		perror("set socket error!\n");
		goto exit;
	}

	if (SOCKET_ERROR == connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
		{
			perror("连接到服务器失败\n");
			goto exit;
		}
	}

	/* 设置超时时间为10s */
	FD_ZERO(&t_set);
	FD_SET(sockfd, &t_set);
	tv.tv_sec = nTimeout;
	tv.tv_usec = 0;

	if (SOCKET_ERROR == select(0, NULL, &t_set, NULL, &tv))
	{
		perror("select socket error!\n");
		goto exit;
	}

	/* 检查是否连接成功 */
	if (!FD_ISSET(sockfd, &t_set))
	{
		perror("connect error!\n");
		goto exit;
	}

	/* 重新设置为阻塞 */
	lRet = 0;
	if (SOCKET_ERROR == ioctlsocket(sockfd, FIONBIO, &lRet))
	{
		perror("set socket error!\n");
		goto exit;
	}

	ret = send(sockfd, send_data, strlen(send_data), 0);
	if (ret < 0)
	{
		printf("发送失败 %d\n", WSAGetLastError());
		goto exit;
	}

	// 等待DUT重连
	begin = clock();

	_sleep(5000);
	/* 在设置请求发送完成后先等待DUT  断开，
	再进行DUT的连接检测*/
	while (TRUE)
	{
		end = clock();
		str.Format("正在重连设备，已过 %d 秒", (end - begin)/1000);
		m_Avg->SetWindowText(str);

		if ((end - begin)/1000 >= MAXDELAYTIME)
		{
			return FALSE;
		}
		if (icmpDll != NULL)
		{
			if (TRUE == runState.stop)
			{
				m_Avg->SetWindowText("");
				return FALSE;
			}

			if (myPing(CON_DUTIP) == TRUE)
			{
				break;
			}

		}
		else
		{
			return FALSE;
		}
		_sleep(500);
	}

exit:
	if (sockfd != INVALID_SOCKET)
		closesocket(sockfd);

	if (ret <= 0)
		return 0;
	else
		return ret;
}


/*******************************************************************************
  Function:    sendWebData
  Description: 发送web请求数据命令
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int sendWebData(const char *pIP, const CString send_data, int nTimeout)
{
	int sockfd = INVALID_SOCKET,ret = 0;
	struct sockaddr_in serverAddr;
	fd_set t_set;
	struct timeval tv;
	unsigned long lRet;

	/* 清除缓冲区 */
	memset(sRecvBuf,0 ,sizeof(sRecvBuf));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("创建socket失败 %d\n", WSAGetLastError());
		goto exit;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(80);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(pIP);

	if(SOCKET_ERROR == setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeout,sizeof(nTimeout)))
	{
		perror("Can't set timeout!\n");
		goto exit;
	}

	/* 设置为非阻塞 */
	lRet = 1;
	if (SOCKET_ERROR == ioctlsocket(sockfd, FIONBIO, &lRet))
	{
		perror("set socket error!\n");
		goto exit;
	}

	if (SOCKET_ERROR == connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
		{
			perror("连接到服务器失败\n");
			goto exit;
		}
	}

	/* 设置超时时间为10s */
	FD_ZERO(&t_set);
	FD_SET(sockfd, &t_set);
	tv.tv_sec = nTimeout;
	tv.tv_usec = 0;

	if (SOCKET_ERROR == select(0, NULL, &t_set, NULL, &tv))
	{
		perror("select socket error!\n");
		goto exit;
	}

	/* 检查是否连接成功 */
	if (!FD_ISSET(sockfd, &t_set))
	{
		perror("connect error!\n");
		goto exit;
	}

	/* 重新设置为阻塞 */
	lRet = 0;
	if (SOCKET_ERROR == ioctlsocket(sockfd, FIONBIO, &lRet))
	{
		perror("set socket error!\n");
		goto exit;
	}

	ret = send(sockfd, send_data, strlen(send_data), 0);
	if (ret < 0)
	{
		printf("发送失败 %d\n", WSAGetLastError());
		goto exit;
	}

	FD_ZERO(&t_set);
	FD_SET(sockfd, &t_set);

	Sleep(1000);
	memset(&tv, 0, sizeof(tv));
	ret = select(sockfd + 1, &t_set, NULL, NULL, &tv);

	if (ret < 0)
	{
		perror("在读取数据报文时SELECT检测到异常\n");
		goto exit;
	}
	else if (ret > 0)
	{
		ret = recv(sockfd, (char*)sRecvBuf, sizeof(sRecvBuf), 0);

		if (ret <= 0)
		{
			perror("读取数据报文时发现远端关闭\n");
			goto exit;
		}
	}

exit:
	if (sockfd != INVALID_SOCKET)
		closesocket(sockfd);

	if (ret <= 0)
		return 0;
	else
		return ret;
}

/*******************************************************************************
  Function:    PageError
  Description: 解析返回的数据包是否包含请求错误的信息
  Author:      liguoliang
  Date:        2014-03-26
*******************************************************************************/
int PageError()
{
	char * ptr;
	int ret = FALSE;
	ptr = strstr(sRecvBuf, "Web Server Error Report");

	if (ptr)
	{
		ret = TRUE;
	}

	return ret;
}

/*******************************************************************************
  Function:    PasssWordWrong
  Description: 解析返回的数据包是否包含需要登录的信息，以此判断cookie是否正确
  Author:      liguoliang
  Date:        2014-03-26
*******************************************************************************/
int PassWordWrong()
{
	char * ptr;
	int ret = FALSE;
	ptr = strstr(sRecvBuf, "/login/loginRpm.css");

	if (ptr)
	{
		ret = TRUE;
	}

	return ret;
}

/*******************************************************************************
  Function:    getCSRFToken
  Description: 解析返回的数据包获取csrt_token的值
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int getCSRFToken(char* csrf_token)
{
	// 解析返回的数据报并读取csrf_token的随机数
	char * ptr;
	int ret = FALSE;
	char tempStr[64], resultStr[16];
	ptr = strstr(sRecvBuf, "csrf_token");

	if (ptr)
	{
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		strcat(tempStr, "csrf_token");
		strcat(tempStr, " = \"\%[^\"]");
		sscanf(ptr, tempStr, resultStr);

		if (strlen(resultStr)>0)
		{
			strcpy(csrf_token, resultStr);
			ret = TRUE;
		}
	}

	return ret;
}

/*******************************************************************************
  Function:    getSessionID
  Description: 解析返回的数据包获取session_id的值
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int getSessionID(char* session_id)
{
	// 解析返回的数据报并读取csrf_token的随机数
	char * ptr;
	int ret = FALSE;
	char tempStr[64], resultStr[16];
	ptr = strstr(sRecvBuf, "session_id");

	if (ptr)
	{
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		strcat(tempStr, "session_id");
		strcat(tempStr, " = \"\%[^\"]");
		sscanf(ptr, tempStr, resultStr);

		if (strlen(resultStr)>0)
		{
			strcpy(session_id, resultStr);
		} else
		{
			strcpy(session_id, "");
		}
		ret = TRUE;
	}

	return ret;
}

/*******************************************************************************
  Function:    getTimeOut
  Description: 解析返回的数据包获取timeOut的值
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int getTimeOut(int nTimeout)
{
	// 解析返回的数据报并读取csrf_token的随机数
	char * ptr;
	int ret = nTimeout;
	char tempStr[64], resultStr[16];
	ptr = strstr(sRecvBuf, "timeOut");

	if (ptr)
	{
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		strcat(tempStr, "timeOut");
		strcat(tempStr, ":\%[^,]");
		sscanf(ptr, tempStr, resultStr);

		if (strlen(resultStr)>4 && strlen(resultStr) < 7)
		{
			ret = atoi(resultStr);
		}
	}

	return ret;
}

char* getArrayElement(char* str, char* result, int index)
{
	if (NULL == str || NULL == result)
	{
		return NULL;
	}

	char* temp = str;
	char* firstColon = NULL;
	char* secondColon = NULL;
	int findNum = 0;

	while('\0' != temp)
	{
		// 先找到第一个"冒号
		if ('\"' == *temp)
		{
			findNum++;
		}
		if (firstColon == NULL && findNum == (index*2-1))
		{
			firstColon = temp+1;
		}
		if (secondColon == NULL && findNum == (index*2))
		{
			secondColon = temp;
			break;
		}
		temp++;
	}

	if ( firstColon != NULL && secondColon != NULL)
	{
		strncpy(result, firstColon, (secondColon - firstColon)/sizeof(char));
		return firstColon;
	}
	else
	{
		return NULL;
	}
}

char* getArrayElement_Marvell(char* str, char* result, int index)
{
	if (NULL == str || NULL == result)
	{
		return NULL;
	}

	char* temp = str;
	char* firstColon = NULL;
	char* secondColon = NULL;
	int findNum = 0;

	while('\0' != temp)
	{
		// 先找到第一个"冒号
		if ('\>' == *temp||'\<' == *temp)
		{
			findNum++;
		}
		if (firstColon == NULL && findNum == (index*2-1))
		{
			firstColon = temp+1;
		}
		if (secondColon == NULL && findNum == (index*2))
		{
			secondColon = temp;
			break;
		}
		temp++;
	}

	if ( firstColon != NULL && secondColon != NULL)
	{
		strncpy(result, firstColon, (secondColon - firstColon)/sizeof(char));
		return firstColon;
	}
	else
	{
		return NULL;
	}
}


/*******************************************************************************
  Function:    getDeviceInfo_7s
  Description: 解析返回的数据包获取7系列mifi产品的硬件版本号
  Author:      liguoliang
  Date:        2014-07-03
*******************************************************************************/
int getDeviceInfo_7s(char *pOut, int nLen, int itemIndex)
{
	/************************************************************************/
	/* 解析返回的数据报并读取序号为itemIndex的元素值(从1开始)
	/* 解析的数据形式为:
	/* var deviceInfoArray = {\n
	\tdevName:"TL-TR761 2000L",\n
	\tsysSoftVer:"1.3.0 Build 130924 Rel.71820n",\n
	\tsysHardVer:"TR761 2000L v1 00000000",\n
	\tmac_str:"08-57-00-36-DE-22",\n
	\timei_str:"A1000039332E2E",\n
	/************************************************************************/

	char * ptr;
	int ret = FALSE;
	char tempStr[100], resultStr[64];
	ptr = strstr(sRecvBuf, "deviceInfoArray");

	if (ptr)
	{
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		ptr = getArrayElement(ptr, resultStr, itemIndex);
		if (ptr ==  NULL)
		{
			return ret;
		}

		if (nLen > strlen(resultStr))
		{
			CString temp;
			temp.Format("%s",resultStr);
			temp.Trim();
			strcpy(pOut, temp.GetBuffer(temp.GetLength()));
			ret = TRUE;
		}
	}

	return ret;
}

/*******************************************************************************
  Function:    getDeviceInfo_8s
  Description: 解析返回的数据包获取8系列mifi产品的硬件版本号
  Author:      liguoliang
  Date:        2014-03-24
*******************************************************************************/
int getDeviceInfo_8s(char *pOut, int nLen, int itemIndex)
{
	/************************************************************************/
	/* 解析返回的数据报并读取序号为itemIndex的元素值(从1开始)
	/* 解析的数据形式为:
	/* var deviceInfoArray = new Array(\n
	/* "M5350", "1.1.3 Build 130814 Rel.47500n ", "M5350 v1 00000000", "5A-8F-CC-EC-66-88", "358688000000234", "0", "0", "11", "0", \n
	/************************************************************************/

	char * ptr;
	int ret = FALSE;
	char tempStr[100], resultStr[64];
	ptr = strstr(sRecvBuf, "deviceInfoArray");

	if (ptr)
	{
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		ptr = getArrayElement(ptr, resultStr, itemIndex);
		if (ptr ==  NULL)
		{
			return ret;
		}

		if (nLen > strlen(resultStr))
		{
			CString temp;
			temp.Format("%s",resultStr);
			temp.Trim();
			strcpy(pOut, temp.GetBuffer(temp.GetLength()));
			ret = TRUE;
		}
	}

	return ret;
}

/*******************************************************************************
  Function:    getDeviceInfo_lte_cmcc
  Description: 解析返回的数据包获取TR961 CMCC设备信息
  Author:      renjain
  Date:        2014-07-03
*******************************************************************************/
int getDeviceInfo_lte_cmcc(const char *pKey, char *pOut, int nLen)
{
	char *ptr;
	char tempStr[64],resultStr[64*4];
	int ret = FALSE;

	ptr = strstr(sRecvBuf, pKey);

	if (ptr)
	{
		// 之前匹配的方法
		#if 0
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		strcat(tempStr, pKey);
		strcat(tempStr, ": \"\%[^\"]");
		sscanf(ptr, tempStr, resultStr);
		#endif

		// 新的匹配方法
		memset(resultStr, 0 ,sizeof(resultStr));
		getArrayElement_Marvell(ptr, resultStr, 1);
		if (nLen < strlen(resultStr))
		{
			CString input = resultStr;
			CString tmp;
			YCodec yCodec;

			// CString to char*
			int strlen = input.GetLength();
			char* str = new char[strlen+1];
			memset(str, 0, strlen+1);
			for(int i=0; i<strlen; i++)
			{
				str[i] = input.GetAt(i);
			}

			// hex string to wchar_t*
			int wchslen = 0;
			if(yCodec.HexStr2WChars(str, strlen, 4, NULL, &wchslen) != -1)
			{
				wchar_t* wchs = new wchar_t[wchslen+1];
				memset(wchs, 0, sizeof(wchar_t)*(wchslen+1));
				int n = yCodec.HexStr2WChars(str, strlen, 4, wchs, &wchslen);
				if(n != -1)
				{
					// wchar_t to CString
					wchar_t* pwch = wchs;
					USES_CONVERSION;
					tmp=W2A(wchs);
				}
				if(wchs)
				{
					delete []wchs;
				}
			}

			if (nLen > tmp.GetLength())
			{
				tmp.Trim();
				strcpy(pOut, tmp.GetBuffer(tmp.GetLength()));
				ret = 1;
			}

			tmp.ReleaseBuffer(0);

		}
		else
		{
			CString temp;
			temp.Format("%s",resultStr);
			temp.Trim();
			//memmove(pOut, resultStr, strlen(resultStr));
			strcpy(pOut, temp.GetBuffer(temp.GetLength()));
			ret = 1;
		}
	}

	return ret;
}

/*******************************************************************************
  Function:    authentication_lte_cmcc
  Description: TR961 CMCC 认证处理
  encrypt_code:0:GET 1:POST
  Author:      renjian
  Date:        2015-01-14
*******************************************************************************/
int authentication_lte_cmcc(const CString nonce, int &nCount, const int encrypt_code, CString &auth_field)
{
	CString nonce_buf = nonce;

	 //认证处理
	int i;
	//CString str;
	CString tmpMd5code;
	CString authRealm = "Highwmg";
	CString authQop = "auth";
	CString HA1;
	CString HA2_2;
	CString authCnonce_f2 = "be8c3e10daa032f5";

	unsigned char encrypt1[] = "admin:Highwmg:admin";
	unsigned char encrypt2[] = "GET:/cgi/xml_action.cgi";
	unsigned char encrypt3[] = "POST:/cgi/xml_action.cgi";
	unsigned char decrypt[HEXLENGTH];

	CString authCount;
	char nCountHex[9];

	CString digestRes2;


	MD5_CTX md5_HA1;
	MD5Init(&md5_HA1);

	//HA1
	MD5Update(&md5_HA1,encrypt1,strlen((char *)encrypt1));
	MD5Final(&md5_HA1,decrypt);

	for(i=0;i<HEXLENGTH;i++)
	{
		tmpMd5code.Format("%02x",decrypt[i]);
		HA1 = HA1 + tmpMd5code;
	}

	//HA2_2
	MD5_CTX md5_HA2_2;
	MD5Init(&md5_HA2_2);

	if (1 == encrypt_code)
	{
		MD5Update(&md5_HA2_2,encrypt3,strlen((char *)encrypt3));
	}
	else
	{
		MD5Update(&md5_HA2_2,encrypt2,strlen((char *)encrypt2));
	}

	MD5Final(&md5_HA2_2,decrypt);

	for(i=0;i<HEXLENGTH;i++)
	{
		tmpMd5code.Format("%02x",decrypt[i]);
		HA2_2 = HA2_2 + tmpMd5code;
	}

	// nc
	ltoa(nCount,nCountHex,HEXLENGTH);
	authCount.Format("%08s",nCountHex);

	nCount += 1;

	//digestRes2
	CString  dig2 = HA1 + ":" + nonce_buf + ":" + authCount + ":" + authCnonce_f2  + ":" + authQop + ":"+ HA2_2;
	unsigned char *puc2 = (unsigned char*)dig2.GetBuffer(dig2.GetLength());

	MD5_CTX md5_digestRes2;
	MD5Init(&md5_digestRes2);
	MD5Update(&md5_digestRes2,puc2,strlen((char *)puc2));
	MD5Final(&md5_digestRes2,decrypt);
	dig2.ReleaseBuffer();

	for(i=0;i<HEXLENGTH;i++)
	{
		tmpMd5code.Format("%02x",decrypt[i]);
		digestRes2 = digestRes2 + tmpMd5code;
	}

	auth_field.Format("%s%s%s%s%s%s%s%s",
		"Authorization:Digest username=\"admin\", realm=\"Highwmg\", nonce=\"",
		nonce_buf.GetBuffer(nonce_buf.GetLength()),
		"\", uri=\"/cgi/xml_action.cgi\", response=\"",
		digestRes2.GetBuffer(digestRes2.GetLength()),
		"\", qop=\"auth\", nc=",
		authCount.GetBuffer(authCount.GetLength()),
		", cnonce=\"",
		authCnonce_f2.GetBuffer(authCnonce_f2.GetLength()));

	return TRUE;

}

/*******************************************************************************
  Function:    deviceInfoInit
  Description: 发送获取设备信息的web请求数据
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
int deviceInfoInit(const char *pIP, const char *pPassword, int nTimeout)
{
	CString sendBuf;
	/*sendBuf.Format("%s%s%s%s%s",
	"GET /deviceInfo/deviceInfo.htm?password=",
	pPassword,
	" HTTP/1.1\r\nHost: ",
	pIP,
	"\r\nUser-Agent: Mozilla/4.0\r\n\r\n\r\n");*/

	sendBuf.Format("%s%s%s%s%s%s%s%s",
			"GET /deviceInfo/deviceInfo.htm?password=",
			pPassword,
			" HTTP/1.1\r\nHost: ",
			pIP,
			"\r\nConnection: keep-alive\r\n",
			"Cache-Control: max-age=0\r\n",
			"User-Agent: Mozilla/4.0\r\n",
			"Cookie: subType=pcSub\r\n\r\n");


	return sendWebData(pIP, sendBuf, nTimeout);
}


/*******************************************************************************
  Function:    fixedBand
  Description: 固定信道，将带宽设置到40M
  Author:      renjian
  Date:        2015-01-04
*******************************************************************************/
int fixedBand(const char *pIP, int nTimeout, const CString nonce, int &nCount, int channel, int bandwidth)
{
	CString sendBuf;
	CString auth_field;

	CString str;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);

	// 认证处理
	authentication_lte_cmcc(nonce, nCount, 1, auth_field);

	/* 配置固定信道、带宽设置为40M */
	char data[MAXPOSTDATASIZE];

	sprintf(data,"<?xml version=\"1.0\"encoding=\"US-ASCII\"?> <RGW><wlan_settings><channel>%d</channel><bandwidth>%d</bandwidth><net_mode>0</net_mode></wlan_settings></RGW>", channel, bandwidth);
	sendBuf.Format("%s%s%s%s%s%s%s%s%s%s%s%d%s%s%s",
		"POST /xml_action.cgi?method=set&module=duster&file=wlan_settings",
		" HTTP/1.1\r\nHost: ",
		pIP,

		"\r\nAccept: application/xml, text/xml, */*; q=0.01\r\n",
		"Referer: /admin.html\r\n",
		"Accept-Language: zh-cn,zh;q=0.8\r\n",
		"X-Requested-With: XMLHttpRequest\r\n",

		auth_field.GetBuffer(auth_field.GetLength()),
		//"Authorization:Digest username=\"admin\", realm=\"Highwmg\", nonce=\"",
		//nonce_buf.GetBuffer(nonce_buf.GetLength()),
		//"\", uri=\"/cgi/xml_action.cgi\", response=\"",
		//digestRes2.GetBuffer(digestRes2.GetLength()),
		//"\", qop=\"auth\", nc=",
		//authCount.GetBuffer(authCount.GetLength()),
		//", cnonce=\"",
		//authCnonce_f2.GetBuffer(authCnonce_f2.GetLength()),

		"\"\r\nContent-Type:  text/xml\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n",
		"Content-Length: ",
		strlen(data),
		"\r\nConnection: Keep-Alive\r\n",
		"Accept-Encoding: gzip, deflate\r\n\r\n",
		data);

	str.Format("正在设置带宽");
	m_Avg->SetWindowText(str);

	if (FALSE != sendWebData_PostBand(pIP, sendBuf, nTimeout))
	{
		/* GET请求需要的认证字段 */
		CString auth_field_GetBandwidth;  // 在获取带宽信息时需要的认证字段

		char bandwidth_str[3];   // 带宽字段 0-13

		str.Format("正在验证带宽设置是否成功");
		m_Avg->SetWindowText(str);

		authentication_lte_cmcc(nonce, nCount, 0, auth_field_GetBandwidth);

		sendBuf.Format("%s%s%s%s%s%s%s",
			"GET /xml_action.cgi?method=get&module=duster&file=wlan_settings",
			" HTTP/1.1\r\nHost: ",
			CON_DUTIP,
			"\r\nConnection: keep-alive\r\n",
			auth_field_GetBandwidth.GetBuffer(auth_field_GetBandwidth.GetLength()),
			"\"\r\nCache-Control: max-age=0\r\n",
			"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n\r\n");

			if (FALSE != sendWebData(CON_DUTIP, sendBuf, nTimeout))
			{
				getDeviceInfo_lte_cmcc("bandwidth", bandwidth_str, sizeof(bandwidth_str));
				if (0 == strcmp(bandwidth_str, "2"))
				{
					g_Dlg->updateCurState(">>带宽设置成功");
					return TRUE;
				}
				else
				{
					g_Dlg->updateCurState(">>带宽设置失败");
					return FALSE;
				}
			}
			else
			{
				g_Dlg->updateCurState(">>获取带宽信息失败");
				return FALSE;
			}
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
  Function:    getDeviceInfo
  Description: 根据键名获取键值
  Author:      liguoliang
  Date:        2013-12-13
*******************************************************************************/
static int getDeviceInfo(const char *pKey, char *pOut, int nLen)
{
	char *ptr;
	char tempStr[64], resultStr[64];
	int ret = 0;

	ptr = strstr(sRecvBuf, pKey);

	if (ptr)
	{
		// 之前匹配的方法
		#if 0
		memset(tempStr, 0, sizeof(tempStr));
		memset(resultStr, 0 ,sizeof(resultStr));
		strcat(tempStr, pKey);
		strcat(tempStr, ": \"\%[^\"]");
		sscanf(ptr, tempStr, resultStr);
		#endif

		// 新的匹配方法
		memset(resultStr, 0 ,sizeof(resultStr));
		getArrayElement(ptr, resultStr, 1);

		if (nLen > strlen(resultStr))
		{
			CString temp;
			temp.Format("%s",resultStr);
			temp.Trim();
			//memmove(pOut, resultStr, strlen(resultStr));
			strcpy(pOut, temp.GetBuffer(temp.GetLength()));
			ret = 1;
		}
	}

	return ret;
}

int getDeviceName(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[0], pOut, nLen);
}

int getDeviceSysSoftVer(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[1], pOut, nLen);
}

int getDeviceSysHardVer(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[2], pOut, nLen);
}

int getDeviceMac(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[3], pOut, nLen);
}

int getDeviceImei(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[4], pOut, nLen);
}

int getDeviceMeid(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[5], pOut, nLen);
}

int getDeviceModemStatus(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[6], pOut, nLen);
}

int getDeviceWorkingMode(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[7], pOut, nLen);
}

int getDevicePin(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[8], pOut, nLen);
}

int getDeviceNonce(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[9], pOut, nLen);
}

int getDeviceId(char *pOut, int nLen)
{
	return getDeviceInfo(sKeyBuf[10], pOut, nLen);
}
