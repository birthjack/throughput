/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   tddp.cpp

  Description: 定义和TDDP发送和接受相关的函数

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-01, liguoliang      create file.
*******************************************************************************/

#include "StdAfx.h"
#include "tddp.h"
#include "des_min_enc.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uchar;
typedef unsigned int uint;


CONVERSATION_TYPE con_type;

#define net_htonl(A)  ((((uint)(A) & 0xff000000) >> 24)  | \
	(((uint)(A) & 0x00ff0000) >> 8 )   | \
	(((uint)(A) & 0x0000ff00) << 8 )   | \
	(((uint)(A) & 0x000000ff) << 24))
#define net_ntohl net_htonl

const int serverPort = TDDP_ROUTER_PORT;

static int checkCPUendian()//返回1，为小端；反之，为大端；
{
	union
	{
		unsigned int  a;
		unsigned char b;
	}c;
	c.a = 1;
	return 1 == c.b;
}

uint tddp_htonl(uint host)
{
	return checkCPUendian() ? net_htonl(host) : host;
}

uint tddp_ntohl(uint net)
{
	return checkCPUendian() ? net_ntohl(net) : net;
}


unsigned int getHexFromChar(const char ch)
{
	unsigned int result = 0;
	if (ch >= 'A' && ch <= 'F')
		result = ch - 55;
	else if (ch >= 'a' && ch <= 'f')
		result = ch - 87;
	else
		result = ch - 48;
	return result;
}

/* 由于是全部变量，多个线程同时运作时，该数据将会是不定状，所以此接口不适用于多线程的同时操作 */
char sPacketBuffer[MAXBUFFSIZE];
char rPacketBuffer[MAXBUFFSIZE];

const int sRetries = 5;
const unsigned short sPacketID = 0x0ff0;

void clearBuffer()
{
	memset(sPacketBuffer, 0, MAXBUFFSIZE);
	memset(rPacketBuffer, 0, MAXBUFFSIZE);
}

void printData(const char *pBuf, int nSize)
{
	for (int i = 0; i < nSize; i++)
		printf("%02x ", pBuf[i]);

	printf("\n");
}

int tddpEncode(const char *pRecvBuf, const char *pKey, char **pRplyBuf)
{
	T_TDDP_HEADER *pTddpHdrRcv;
	uchar md5Res[DIGEST_LENGTH + 1];
	uchar desRes[1024];
	int desLen, nBufSize;
	const uint tddpExHdrSize = TDDP_V2_HDR_LEN;

	pTddpHdrRcv = (T_TDDP_HEADER *)pRecvBuf;
	memset((char *)pRecvBuf + sizeof(T_TDDP_HEADER), 0, DIGEST_LENGTH);

	desLen = tddp_ntohl(pTddpHdrRcv->PktLength);
	if (desLen)
	{
		if (pTddpHdrRcv->Type == CMD_SYS_OPR)
			desLen += 7;
		else
			desLen += 9;

		desLen = ((desLen >> 3) << 3);//8字节对齐
	}
	pTddpHdrRcv->PktLength = tddp_htonl(desLen);

	memset(md5Res, 0, sizeof(md5Res));
	make_md5_digest(md5Res, (unsigned char*)pRecvBuf, tddpExHdrSize + desLen);
	memcpy((char *)pRecvBuf + sizeof(T_TDDP_HEADER), md5Res, DIGEST_LENGTH);

	if (desLen == 0)
	{
		nBufSize = tddpExHdrSize;
	}
	else
	{
		memset(md5Res, 0, sizeof(md5Res));
		make_md5_digest(md5Res, (uchar *)pKey, strlen(pKey));

		nBufSize = des_min_do((unsigned char *)pRecvBuf + tddpExHdrSize,
			desLen, desRes, sizeof(desRes), md5Res, DES_ENCRYPT);

		if (nBufSize > 0 && nBufSize == desLen)
		{
			memcpy((char *)pRecvBuf + tddpExHdrSize, desRes, desLen);

			if (pRplyBuf)
				*pRplyBuf = (char *)pRecvBuf;

			nBufSize += tddpExHdrSize;
		}
	}

	return nBufSize;
}

int tddpDecode(const char *pRecvBuf, const char *pKey, char *pRplyBuf)
{
	if (!pRecvBuf || !pKey || !pRplyBuf)
		return -1;
	else
	{
		uchar md5_digest[DIGEST_LENGTH + 1];
		T_TDDP_HEADER *pTddpHdrRecv, *pTddpHdrRply;
		uint tddpDataSize;
		uint nBufSize;
		const uint tddpExHdrSize = TDDP_V2_HDR_LEN;

		pTddpHdrRecv = (T_TDDP_HEADER *)pRecvBuf;
		pTddpHdrRply = (T_TDDP_HEADER *)pRplyBuf;

		memcpy(pTddpHdrRply, pTddpHdrRecv, tddpExHdrSize);
		tddpDataSize = tddp_ntohl(pTddpHdrRecv->PktLength);

		memset(md5_digest, 0, sizeof(md5_digest));
		make_md5_digest(md5_digest, (uchar *)pKey, strlen(pKey));

		nBufSize = des_min_do((uchar *)pRecvBuf + tddpExHdrSize, tddpDataSize, (uchar *)pRplyBuf + tddpExHdrSize,
			MAXBUFFSIZE - tddpExHdrSize, md5_digest, DES_DECRYPT);

		memset(md5_digest, 0, sizeof(md5_digest));
		memcpy(md5_digest, pRplyBuf + sizeof(T_TDDP_HEADER), DIGEST_LENGTH);
		memset(pRplyBuf + sizeof(T_TDDP_HEADER), 0, DIGEST_LENGTH);

		if (FALSE == verify_md5_digest(md5_digest, (uchar *)pRplyBuf, nBufSize + tddpExHdrSize))
		{
			printf("Decode error\n");
			return -1;
		}
		else
		{
			*(pRplyBuf + tddpExHdrSize + tddpDataSize) = NULL;
			return 0;
		}
	}
}

int SendPacket(SOCKET *pSocket, sockaddr_in* hostAddr, const char *pKey)
{
	T_TDDP_HEADER *pTddpHdr;
	int result = MSG_SUCCESS;
	int nBufSize = 0;
	const uint tddpExHdrSize =  TDDP_HDR_LEN;
	
	pTddpHdr = (T_TDDP_HEADER *)sPacketBuffer;

	unsigned int data_len = ntohl(pTddpHdr->PktLength);

	if ( TDDP_VER_TWO == pTddpHdr->Ver)
	{
		nBufSize = tddpEncode(sPacketBuffer, pKey, NULL);
	} else
	{
		nBufSize = tddpExHdrSize+ data_len;
	}

	int len = sizeof(*hostAddr);
	result = sendto(*pSocket, sPacketBuffer, nBufSize, 0, (struct sockaddr*)hostAddr, len);
	result = (result == -1 ? MSG_ERROR : MSG_SUCCESS);

	return result;
}

int recvPacket(SOCKET *pSocket,sockaddr_in *pClientAddr, const char *pKey, char *pResultBuf, int timeOut)
{
	int ret = MSG_SUCCESS;
	T_TDDP_HEADER *pTddpHdr;

	const uint tddpExHdrSize = TDDP_V2_HDR_LEN;

	fd_set readfds;
	struct timeval tv;
	
	if(pSocket)
	{
		tv.tv_sec = timeOut;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(*pSocket, &readfds);
		ret = select(*pSocket+(SOCKET)1, &readfds, NULL, NULL, &tv);

		if (ret)
		{
			memset(rPacketBuffer, 0, sizeof(rPacketBuffer));

			int clientlen = sizeof(*pClientAddr);
			ret = recvfrom(*pSocket, rPacketBuffer, MAXBUFFSIZE, 0, 
				(struct sockaddr*)pClientAddr, &clientlen);

			if (ret > 0)
			{
				ret = MSG_SUCCESS;
				pTddpHdr = (T_TDDP_HEADER *)rPacketBuffer;
				if (pTddpHdr->ReplyInfo != CMD_EXE_SUCCESS)
				{
					ret = MSG_ERROR;
				}else
				{
					if (pTddpHdr->Ver == TDDP_VER_TWO)
					{
						if ( NULL == pResultBuf || tddpDecode(rPacketBuffer, pKey, pResultBuf) != 0)
						{
							ret = MSG_ERROR;
						}
					} 
					else
					{
						memcpy(pResultBuf, rPacketBuffer, sizeof(T_TDDP_HEADER) + pTddpHdr->PktLength);
					}

					if (ret == MSG_SUCCESS)
					{
						if (pTddpHdr->Code != con_type.code && pTddpHdr->SubType != con_type.subType)
							ret = MSG_TYPE_ERROR;
						else if (pTddpHdr->PktID != sPacketID)
							ret = MSG_PACKET_NOT_COMPAIRE;
						else if (pTddpHdr->ReplyInfo != CMD_EXE_SUCCESS)
							ret = MSG_ERROR;
						else
						{
							
							ret = MSG_SUCCESS;
						}

					} else
					{
						ret = MSG_ERROR;
					}
				}
			}
			else
			{
				ret = MSG_ERROR;
			}
		} else
		{
			ret = MSG_ERROR;
		}

	} else
	{
		ret = MSG_ERROR;
	}

	return ret;
}