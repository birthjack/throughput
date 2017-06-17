/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   tddpDeviceInfo.cpp

  Description: ͨ������TDDPЭ�����ݻ�ȡ�豸��Ϣ��صĺ���

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-11-26, liguoliang      create file.
*******************************************************************************/

#include "StdAfx.h"
#include "tddpDeviceInfo.h"


/* ��ʼ��socket�׽��� */
int initSocket(SOCKET *sock, sockaddr_in *client, sockaddr_in *server, char* server_ip)
{
	int ret = 1;
	client->sin_family = AF_INET;
	client->sin_addr.s_addr = htonl(INADDR_ANY);
	client->sin_port = htons(TDDP_PORT);

	if(bind(*sock, (struct sockaddr*)client, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		
		ret = 0;
	}

	server->sin_family = AF_INET;
	server->sin_addr.S_un.S_addr = inet_addr(server_ip);
	server->sin_port= htons(TDDP_ROUTER_PORT);

	return ret;
}

/* ͨ������TDDP���ݿ�����ر�WiFi */
int controlWiFi(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
				const char *pKey, char *pResultData, int is_open)
{
	int ret = MSG_SUCCESS;

	T_TDDP_HEADER *pTddpHdr;
	int nBufSize;

	const unsigned int tddpExHdrSize = TDDP_V2_HDR_LEN;

	if (pSocket)
	{
		clearBuffer();

		// �������ݰ�����
		if (1 == is_open)
		{
			con_type.subType = CMD_WLAN_ENABLE;
		} else
		{
			con_type.subType = CMD_WLAN_DISABLE;
		}
		con_type.code = TDDP_REQUEST;
		con_type.type = CMD_SYS_OPR;
		

		char* tddp_data = NULL;
		unsigned int data_len = 0;
		pTddpHdr = (T_TDDP_HEADER *)sPacketBuffer;
		pTddpHdr->Ver = TDDP_VER_TWO;		
		pTddpHdr->Type = con_type.type;
		pTddpHdr->Code = con_type.code;
		pTddpHdr->ReplyInfo = 0;
		pTddpHdr->Reserve = 0;
		pTddpHdr->PktID = sPacketID;
		pTddpHdr->SubType = con_type.subType;

		if (data_len)
		{
			memcpy(sPacketBuffer + tddpExHdrSize, tddp_data, data_len);
			if (CMD_SYS_OPR != pTddpHdr->SubType)
			{
				memcpy(sPacketBuffer + tddpExHdrSize + data_len, "\r\n", 2);
				data_len += 2;
			}
		}
		pTddpHdr->PktLength = htonl(data_len);


		ret = SendPacket(pSocket, sendAddr, pKey);

		if (ret == MSG_SUCCESS)
		{
			con_type.code = TDDP_REPLY;
			ret = recvPacket(pSocket, receiveAddr, pKey, pResultData, 8);
			Sleep(3000);
		}
		
	} else
	{
		ret = MSG_ERROR;
	}

	return ret;
}

/* ͨ������TDDP���ݻ�ȡIMEI�� */
int getIMEIInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData)
{
	int ret = 1;

	return ret;
}

/* ͨ������TDDP���ݻ�ȡ����汾�� */
int getSoftwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData)
{
	int ret = 1;

	return ret;
}

/* ͨ������TDDP���ݻ�ȡӲ���汾�� */
int getHardwareVersionInfo(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData)
{
	int ret = 1;

	return ret;
}

/* ����TDDPͬ��ָ������ */
int tddpSync(SOCKET *pSocket, sockaddr_in* sendAddr, sockaddr_in* receiveAddr,
	const char *pKey, char *pResultData)
{
	int ret = MSG_SUCCESS;

	T_TDDP_HEADER *pTddpHdr;
	int nBufSize;

	const unsigned int tddpExHdrSize = TDDP_V2_HDR_LEN;

	if (pSocket)
	{
		clearBuffer();

		// �������ݰ�����
		con_type.subType = CMD_SYS_INIT;
		con_type.code = TDDP_REQUEST;
		con_type.type = CMD_SYS_OPR;

		char* tddp_data = NULL;
		unsigned int data_len = 0;
		pTddpHdr = (T_TDDP_HEADER *)sPacketBuffer;
		pTddpHdr->Ver = TDDP_VER_TWO;
		pTddpHdr->Type = con_type.type;
		pTddpHdr->Code = con_type.code;
		pTddpHdr->ReplyInfo = 0;
		pTddpHdr->Reserve = 0;
		pTddpHdr->PktID = sPacketID;
		pTddpHdr->SubType = con_type.subType;

		if (data_len)
		{
			memcpy(sPacketBuffer + tddpExHdrSize, tddp_data, data_len);
			if (CMD_SYS_OPR != pTddpHdr->SubType)
			{
				memcpy(sPacketBuffer + tddpExHdrSize + data_len, "\r\n", 2);
				data_len += 2;
			}
		}
		pTddpHdr->PktLength = htonl(data_len);


		ret = SendPacket(pSocket, sendAddr, pKey);

		if (ret == MSG_SUCCESS)
		{
			con_type.subType = CMD_AUTO_TEST;
			con_type.code = TDDP_REQUEST;
			ret = recvPacket(pSocket, receiveAddr, pKey, pResultData, 8);
		}

	} else
	{
		ret = MSG_ERROR;
	}

	return ret;
}