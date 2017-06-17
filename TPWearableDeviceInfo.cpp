/*******************************************************************************
 Copyright (C), 2016 TP-LINK TECHNOLOGIES CO., LTD.

 File name:   TPWearableDeviceInfo.cpp

 Description: 通过TPWearable远程接口获取设备信息的实现文件

 Author:      Liu Kai <liuKai_y0152@tp-link.net>

 History:
 ---------------------------------
 V1.0, 2016-01-27, Liu Kai      create file.
*******************************************************************************/
#include "StdAfx.h"
#include "TPWearableDeviceInfo.h"
#include <WinSock2.h>
#include <stdio.h>
#include <string.h>


#define MAX_TPWBUF_SIZE 2048
#define TOKEN_BUF_SIZE 64

#define TPW_HTTP_PORT_NUM 9999

#define STARTDIALOG_MSG_ID "536870913"
#define STOPDIALOG_MSG_ID "536870914"
#define GETDEVINFO_MSG_ID "536870921"
#define RESET_MSG_ID "536870920"
#define IPERF_MSG_ID "131073"

static char g_Json[MAX_TPWBUF_SIZE] = {0};
static char KeyString[][32] = {"imei","meid","sw_version","hw_version","pin","wlan_mac","workingMode","modemStatus","device_id","nonce"};

enum KeyStringNum
{
   IMEI_STR_NUM,
   MEID_STR_NUM,
   SOFTVER_STR_NUM,
   HARDVER_STR_NUM,
   PIN_STR_NUM,
   MAC_STR_NUM,
   WM_STR_NUM,
   MS_STR_NUM,
   DEVID_STR_NUM
};

typedef enum  TPWRequestTypeEnum
{
   TPW_StartDialog = 0, // 发起会话
   TPW_GetDeviceInfo = 1, // 获取设备信息
   TPW_StopDialog = 2, // 终止会话
   TPW_Reset = 3, //恢复出厂设置
   TPW_Type_NUM //已定义请求类型数目
}TPWRequestType;

/*******************************************************************************
 Function:    SendData
 Description: 通过socket发送数据，并保存接收数据
 Input:       pIP, 对端IP
              SendData, 发送数据
              pRecieveData, 保存接收数据的缓存地址
              nTimeout, 超时时间

 Output:      输出错误信息到标准输出
 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int SendData(const char *pIP, const CString SendData, char* pRecieveData, int RecvLen, int nTimeout = 10)
{
	int sockfd = INVALID_SOCKET;
	int ret = 0;
	struct sockaddr_in serverAddr;
	fd_set t_set;
	struct timeval tv;
	unsigned long lRet;


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("创建socket失败 %d\n", WSAGetLastError());
		goto exit;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(TPW_HTTP_PORT_NUM); //TPWearable http server port
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

	ret = send(sockfd, SendData, strlen(SendData), 0);
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
		/* 清除缓冲区 */
		memset(pRecieveData, 0, RecvLen);
		ret = recv(sockfd, pRecieveData, RecvLen, 0);

		if (ret <= 0)
		{
			perror("读取数据报文时发现远端关闭\n");
			goto exit;
		}
	}

exit:
	if (sockfd != INVALID_SOCKET)
	{
		closesocket(sockfd);
	}

	if (ret <= 0)
	{
		return 0;
	}
	else
	{
		return ret;
	}
}

/*******************************************************************************
 Function:    GetKeyStrFromJson
 Description: 从Json字段字符串中查找关键字符串获取其值
 Input:       pKey, 查找的关键字串
              pJson, 查找的源字串
              pOut, 保存获取值的缓存地址

 return:      1 查找到, 0 查找不到
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int GetKeyStrFromJson(const char *pKey, const char *pJson, char* pOut)
{
	const char *pStr;
	char TempStr[64] = {0};
	char TempOut[64] = {0};

	pStr = strstr(pJson, pKey);

	if (NULL != pStr)
	{
		// 按pKey字符串+":%[^,]格式匹配，获取：与，之间字符
		strcat(TempStr, pKey);
		strcat(TempStr, "\":%[^,}]");
		sscanf(pStr, TempStr, TempOut);

		int Len = strlen(TempOut);
		if (TempOut[0] == '\"' && TempOut[Len-1] == '\"')
		{
			memcpy(pOut, TempOut+1, Len-2);
		}
		else
		{
			memcpy(pOut, TempOut, Len);
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
 Function:    GetRval
 Description: 获取命令应答Json中的错误码rval字段
 Input:       pJson, 命令Json
 return:      1, 获取错误码成功，错误码为0
              0, 获取错误码成功，错误码不为0
              -1, 获取错误码失败

 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int GetRval(const char *pJson)
{
	char RvalBuf[16] = {0};

	if (0 == GetKeyStrFromJson("rval", pJson, RvalBuf))
	{
		return -1;
	}
	else if (0 == strcmp(RvalBuf, "0"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
 Function:    GetToken
 Description: 获取StartDialog应答Json中的会话标识token字段
 Input:       pJson, 查找源字符串
              pToken, 保存获取值的缓存地址

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int GetToken(const char *pJson, char *pToken)
{
	if (0 == GetKeyStrFromJson("token", pJson, pToken))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*******************************************************************************
 Function:    SendHttpRequest
 Description: 向TPWearable发起http请求，正文为JSON数据
 Input:       pIP, 对端IP
              pText, 填充到http正文中的JSON数据
              pHttpRecv, 存放接收到的Http数据的缓存地址

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int SendHttpRequest(const char *pIP, const char *pText, char *pHttpRecv, int RecvLen)
{
	CString DataBuf;

	DataBuf.Format("%s%s%s%s%s%d%s%s%s%d%s%s",
			"POST / HTTP/1.1\r\n",
			"Connection: Close\r\n",
			"Content-type: application/x-www-form-urlencoded\r\n",
			"charset: UTF-8\r\n",
			"Content-Length: ",
			strlen(pText),
			"\r\nHost: ",
			pIP,
			":",
			TPW_HTTP_PORT_NUM,
			"Accept: */*\r\n\r\n",
			pText);

	if (0 == SendData(pIP, DataBuf, pHttpRecv, RecvLen))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*******************************************************************************
 Function:    HttpResOK
 Description: 根据http应答头判断http请求是否成功
 Input:       pResponseData, http应答数据
 return:      1 if Response OK, 0 if not
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int HttpResOK(char *pResponseData)
{
	if (NULL != strstr(pResponseData, "200 OK"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
 Function:    AnalyzeHttpResponse
 Description: 解析http应答数据，得到http应答正文
 Input:       pResponseData, http应答数据
              pTextBuf, http正文的缓存地址

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
static int AnalyzeHttpResponse(const char *pResponseData, char *pTextBuf)
{
	const char *pStr;
	pStr = strstr(pResponseData, "\r\n\r\n");
	if (NULL != pStr)
	{
		sscanf(pStr, "\r\n\r\n{%[^}]", pTextBuf);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
 Function:     SendTPWRequest
 Description:  通过Http向TPWearable发起命令请求，暂未启用，各命令有各自的函数实现
 Input:        pIP, http服务器端IP
               pToken, 命令请求需携带的token字段
               pJsonRecieve, 存放应答数据的地址
               Type, 命令请求消息类型

 return:       1 if success, 0 if fail
 Author:       Liu Kai
 Date:         2016-01-27
*******************************************************************************/
static int SendTPWRequest(const char *pIP, char *pToken, char *pJsonRecieve, TPWRequestType Type)
{
	char RecvBuf[MAX_TPWBUF_SIZE] = {0};
	char SendBuf[MAX_TPWBUF_SIZE] = {0};

	//TODO cam_pass,user_name,user_pass待完善
	switch (Type)
	{
	case TPW_StartDialog:
		_snprintf(SendBuf, sizeof(SendBuf),
			"{\"seq\":1,\"msg_id\":%s,\"cam_pass\":\"\",\"user_name\":\"whatname\",\"user_pass\":\"\"}", STARTDIALOG_MSG_ID);
		break;
	case TPW_GetDeviceInfo:
		_snprintf(SendBuf, sizeof(SendBuf), "{\"seq\":1,\"msg_id\" :%s,\"token\":\"%s\",\"fields\":[]}", GETDEVINFO_MSG_ID, pToken);
		break;
	case TPW_StopDialog:
		_snprintf(SendBuf, sizeof(SendBuf), "{\"seq\":1,\"msg_id\":%s,\"token\":\"%s\"}", STOPDIALOG_MSG_ID, pToken);
		break;
	case TPW_Reset:
		_snprintf(SendBuf, sizeof(SendBuf), "{\"seq\":1,\"msg_id\":%s,\"token\":\"%s\",\"type\":\"reset\"}", RESET_MSG_ID, pToken);
	default:
		break;
	}

	if (0 == SendHttpRequest(pIP, SendBuf, RecvBuf, sizeof(RecvBuf)))
	{
		perror("Http request fail!\n");
		return 0;
	}

	if (0 == HttpResOK(RecvBuf))
	{
		perror("Http response is not 200 OK!\n");
		return 0;
	}

	AnalyzeHttpResponse(RecvBuf, pJsonRecieve);

	if (1 == GetRval(pJsonRecieve))
	{
		return 1;
	}
	else
	{
		perror("Get rval fail or rval is -1\n");
		return 0;
	}
}

static int StartIperf(const char *pIP, const char *pToken, const char *test_in_dut, char *pJsonRecieve)
{
	char RecvBuf[MAX_TPWBUF_SIZE]={0};
	char SendBuf[MAX_TPWBUF_SIZE]={0};

	_snprintf(SendBuf, sizeof(SendBuf),
	"{\"seq\":1,\"msg_id\":%s,\"token\":\"%s\",\"type\":\"iperf\",\"param\": \"%s\"}", IPERF_MSG_ID, pToken, test_in_dut);

	if (0==SendHttpRequest(pIP, SendBuf, RecvBuf, sizeof(RecvBuf)))
	{
		//TODO 输出错误信息
		return 0;
	}

	if(0== HttpResOK(RecvBuf))
	{
		//TODO 输出错误信息
		return 0;
	}

	AnalyzeHttpResponse(RecvBuf, pJsonRecieve);

	if (1 == GetRval(pJsonRecieve))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*******************************************************************************
 Function:    ResetTPWDevice
 Description: 通过命令恢复TPWearable设备出厂设置
 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
int ResetTPWDevice(const char *pIP)
{
	char JsonBuf[MAX_TPWBUF_SIZE] = {0};
	char TokenBuf[TOKEN_BUF_SIZE] = {0};

	if (0 == SendTPWRequest(pIP, NULL, JsonBuf, TPW_StartDialog))
	{
		perror("TPWRequest StartDialog failed!\n");
		return 0;
	}

	if (0 == GetToken(JsonBuf, TokenBuf))
	{
		perror("TPWRequest GetToken failed!\n");
		return 0;
	}

	if (0 == SendTPWRequest(pIP, TokenBuf, JsonBuf, TPW_Reset))
	{
		perror("TPWRequest Reset failed!\n");
		return 0;
	}

	return 1;
}

/*******************************************************************************
 Function:    GetTPWearableInfo
 Description: 通过命令请求获取TPWearable设备信息并保存，供外部文件中调用的接口函数
 Input:       None
 OutPut：     存放设备信息Jason到全局数组g_Json

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
int GetTPWearableInfo(const char *pIP)
{
	char JsonBuf[MAX_TPWBUF_SIZE] = {0};
	char TokenBuf[TOKEN_BUF_SIZE] = {0};

	if (0 == SendTPWRequest(pIP, NULL, JsonBuf, TPW_StartDialog))
	{
		perror("TPWRequest StartDialog failed!\n");
		return 0;
	}

	if (0 == GetToken(JsonBuf, TokenBuf))
	{
		perror("Get Token failed!\n");
		return 0;
	}

	memset(JsonBuf, 0, sizeof(JsonBuf));

	if (0 == SendTPWRequest(pIP, TokenBuf, JsonBuf, TPW_GetDeviceInfo))
	{
		perror("TPWRequest GetDeviceInfo failed!\n");
		return 0;
	}

	memset(g_Json, 0, sizeof(g_Json));
	memcpy(g_Json, JsonBuf, sizeof(JsonBuf));

	memset(JsonBuf, 0, sizeof(JsonBuf));
	if (0 == SendTPWRequest(pIP, TokenBuf, JsonBuf, TPW_StopDialog))
	{
		perror("TPWRequest StopDialog failed!\n");
		return 0;
	}
	return 1;
}

int StartTPWearableIperf(const char *test_in_dut, const char *pIP)
{
	char JsonBuf[MAX_TPWBUF_SIZE]={0};
	char TokenBuf[64]={0};

	if (0 == SendTPWRequest(pIP, NULL, JsonBuf, TPW_StartDialog))
	{
		perror("TPWRequest StartDialog failed!\n");
		return 0;
	}

	if (0 == GetToken(JsonBuf, TokenBuf))
	{
		return 0;
	}

	memset(JsonBuf, 0 ,sizeof(JsonBuf));

	if (0 == StartIperf(pIP, TokenBuf, test_in_dut, JsonBuf))
	{
		return 0;
	}

	memset(g_Json, 0 ,sizeof(g_Json));
	memcpy(g_Json, JsonBuf, sizeof(JsonBuf));
	memset(JsonBuf, 0, sizeof(JsonBuf));

	if (0 == SendTPWRequest(pIP, TokenBuf, JsonBuf, TPW_StopDialog))
	{
		perror("TPWRequest StopDialog failed!\n");
		return 0;
	}
	return 1;

}


//获取设备各信息的函数，供外部文件中调用
int GetTPWearableIMEI(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[IMEI_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableMEID(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[MEID_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableSoftVer(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[SOFTVER_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableHardVer(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[HARDVER_STR_NUM], g_Json, OutBuf);
}

int GetTPWearablePin(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[PIN_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableMac(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[MAC_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableWorkingMode(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[WM_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableModemStatus(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[MS_STR_NUM], g_Json, OutBuf);
}

int GetTPWearableDeviceId(char *OutBuf)
{
	return GetKeyStrFromJson(KeyString[DEVID_STR_NUM], g_Json, OutBuf);
}
