/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   tddpDeviceInfo.h

  Description: ͨ������TDDPЭ�����ݻ�ȡ�豸��Ϣ��صĺ���

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-11-26, liguoliang      create file.
*******************************************************************************/

#ifndef _TDDPDEVICEINFO_H_
#define _TDDPDEVICEINFO_H_

#include "tddp.h"

extern CONVERSATION_TYPE con_type;

/* ��ʼ��socket�׽��� */
int initSocket(SOCKET *sock, sockaddr_in *client, sockaddr_in *server, char* server_ip);

/* ͨ������TDDP���ݿ�����ر�WiFi */
int controlWiFi(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData, int is_open);

/* ͨ������TDDP���ݻ�ȡIMEI�� */
int getIMEIInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* ͨ������TDDP���ݻ�ȡ����汾�� */
int getSoftwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* ͨ������TDDP���ݻ�ȡӲ���汾�� */
int getHardwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

/* ����TDDPͬ��ָ������ */
int tddpSync(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData);

#endif