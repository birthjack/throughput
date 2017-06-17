/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   tddpDeviceInfo.h

  Description: 通过发送TDDP协议数据获取设备信息相关的函数

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-11-26, liguoliang      create file.
*******************************************************************************/

#ifndef _TDDPDEVICEINFO_H_
#define _TDDPDEVICEINFO_H_

#include "tddp.h"

extern CONVERSATION_TYPE con_type;

/* 初始化socket套接字 */
int initSocket(SOCKET *sock, sockaddr_in *client, sockaddr_in *server, char* server_ip);

/* 通过发送TDDP数据开启或关闭WiFi */
int controlWiFi(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData, int is_open);

/* 通过发送TDDP数据获取IMEI号 */
int getIMEIInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* 通过发送TDDP数据获取软件版本号 */
int getSoftwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* 通过发送TDDP数据获取硬件版本号 */
int getHardwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* 发送TDDP同步指令数据 */
int tddpSync(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

#endif