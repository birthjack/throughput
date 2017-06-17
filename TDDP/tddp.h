#ifndef _TDDP_H_
#define _TDDP_H_

#include "crypto_min_inc.h"

#define WM_CON_NEW				WM_USER+100
#define WM_CON_UPDATE			WM_USER+101
#define WM_CON_EXIT				WM_USER+102
#define WM_MAC_CONF_SUCCESS		WM_USER+103
#define WM_TEST_CONF_FAIL		WM_USER+104
#define WM_CON_INITMSG			WM_USER+105
#define WM_CON_TIMEOUT			WM_USER+106

#define WM_PIN_CONF_SUCCESS     WM_USER+107
#define WM_CON_NTWKERR          WM_USER+108

#define TDDP_SIGN_MAX_LEN 256
#define TDDP_PORT 61000
#define TDDP_ROUTER_PORT 1040

/* Version define */
#define TDDP_VER				1
#define TDDP_VER2				2

/* Version define */
#define TDDP_VER_ONE	1
#define TDDP_VER_TWO    2

/* Code define */
#define TDDP_REQUEST 			1
#define TDDP_REPLY 				2

/* Type define */
#define TYPE_SET_CFG 			1
#define TYPE_GET_CFG 			2
#define TYPE_SPE_CMD			3
#define TYPE_HEART_BEAT         4

/* Type define */
#define SET_USR_CFG 			1
#define GET_SYS_INF 			2
#define CMD_SYS_OPR 			3
#define CMD_AUTO_TEST			4
#define CMD_GET_SIGN			5
#define CMD_CONFIG_MAC 			6
#define CMD_CANCEL_TEST 		7
#define CMD_REBOOT_FOR_TEST		8
#define CMD_CLEAR_INFO			9
#define CMD_GET_PROD_ID			10
#define CMD_SET_SIGN			11
#define CMD_SYS_INIT			12

#define CMD_CONFIG_PIN          13


#define CMD_WAN_PHLINK			14
#define CMD_WAN_LOGICLINK		15
#define CMD_WAN_CONN			16
#define CMD_ERASE_WLAN_CAL_DATA	17/*  by huangwenzhong, 15Mar13 */
#define CMD_CONFIG_MODEM_NV		18/*  by luowei, 1Aug13 */
#define CMD_WLAN_DISABLE        25
#define CMD_WLAN_ENABLE         26

/* Code define */
#define TDDP_REQUEST 			1
#define TDDP_REPLY 				2

/* Reply information define */
#define REPLY_OK 				0
#define REPLY_ERROR				-1

#define		CMD_EXE_SUCCESS		 0    /* 命令执行成功 */
#define		CMD_PARAM_OVERFLOW	 1    /* 命令解析成功，但出现参数溢出丢弃，如数组长度 */
#define		CMD_ERROR			 2    /* 命令解析错误 */
#define		CMD_EXE_ERROR		 3    /* 命令执行错误 */
#define		CMD_DEFAULT_ERROR	 4    /* 出现未知错误 */
#define		CMD_ENTRY_OVERFLOW	 5    /* 命令解析成功，但条数出线溢出 */
#define		CMD_PKT_ERROR 		 6
#define		CMD_VER_ERROR		 7	  /*版本错误*/

#pragma pack(push, 1)
typedef struct 
{
	unsigned char 	Ver;			/* tddp version */
	unsigned char 	Type;			/* data type: 1--set, 2--get, 3--cmd */
	unsigned char	Code;			/* packet code: 1--request, 2--reply */
	unsigned char	ReplyInfo;		/* reply infomation: 0--ok, others(-1,2...)--error */
	unsigned int	PktLength;		/* the length of tddp packet, not including the header */
	unsigned short 	PktID;			/* packet ID, reply's ID same to request's */
	unsigned char	SubType;	/* fragment flag:  0--not fragment, 1--more fragments */
	unsigned char	Reserve;		/* reserve */
}T_TDDP_HEADER;
#pragma pack(pop)

//typedef struct
//{
//	unsigned char mac[8];
//	UINT32 data;	
//}TDDP_MAC_CONF_STRUCT;

typedef struct
{
	unsigned char dsaSign[TDDP_SIGN_MAX_LEN];
	unsigned long signLen;
}TDDP_SIGN_CONF_STRUCT;

#define MAXBUFFSIZE 1024
#define TDDP_HDR_LEN    (sizeof(T_TDDP_HEADER))
#define TDDP_V2_HDR_LEN (sizeof(T_TDDP_HEADER) + DIGEST_LENGTH)

#define PROD_FLAG_MAC			0x0001
#define PROD_FLAG_PROD_ID		0x0002
#define PROD_FLAG_FLASH_SN		0x0004
#define PROD_FLAG_SIGN			0x0008
#define PROD_FLAG_DATE			0x0010
#define PROD_FLAG_TEST			0x0020
#define PROD_FLAG_SIGN_BY_MAC	0x0040		/* SIGN 必须跟 MAC 同时选择, by lqm, 21Jun06 */
#define PROD_FLAG_PIN           0x0080     // to config PIN

#define PROD_FLAG_FULL			(PROD_FLAG_MAC | PROD_FLAG_PROD_ID | PROD_FLAG_FLASH_SN | PROD_FLAG_SIGN | PROD_FLAG_DATE | PROD_FLAG_TEST)
#define PROD_FLAG_FULL_NO_TEST	(PROD_FLAG_MAC | PROD_FLAG_PROD_ID | PROD_FLAG_FLASH_SN | PROD_FLAG_SIGN | PROD_FLAG_DATE)
#define PROD_FLAG_FULL_NO_MAC	(PROD_FLAG_PROD_ID | PROD_FLAG_FLASH_SN | PROD_FLAG_SIGN | PROD_FLAG_DATE | PROD_FLAG_TEST)
#define PROD_FLAG_OTHER			(PROD_FLAG_MAC)
enum WRITE_TYPE
{
	WRITE_TYPE_COM = 0x01,
	WRITE_TYPE_SOCKET = 0x02,
	WRITE_TYPE_BOTH = WRITE_TYPE_COM | WRITE_TYPE_SOCKET,
};



typedef struct
{
	unsigned char mac[8];
	unsigned int data;
} TDDP_MAC_CONF_STRUCT;

typedef struct
{
	unsigned char nv[15]; /* IMEI:15; MEID:14 */
	unsigned char nvType; /* 0:IMEI; 1:MEID*/
} TDDP_NV_CONF_STRUCT;

typedef struct
{
	int type;
	int code;
	int subType;
} CONVERSATION_TYPE;

typedef struct
{
	char *buf/*[MAXBUFFSIZE]*/;
	int bufSize;
	CONVERSATION_TYPE *convTy;
} packetBufType;

enum {
	MSG_ERROR = -1,
	MSG_SUCCESS = 0,
	MSG_TIMEOUT,
	MSG_PRODUCTID_NOT_COMPAIRE,
	MSG_PACKET_NOT_COMPAIRE,
	MSG_BROADCAST_NOT_RESPONSE,
	MSG_TYPE_ERROR,
	MSG_FAIL
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern char sPacketBuffer[MAXBUFFSIZE];
	extern char rPacketBuffer[MAXBUFFSIZE];
	
	extern const int sRetries;
	extern const unsigned short sPacketID;
	extern const int serverPort;

	void clearBuffer();
	void printData(const char *pBuf, int nSize);

	unsigned int getHexFromChar(const char ch);
	int tddpEncode(const char *pRecvBuf, const char *pKey, char **pRplyBuf);
	int tddpDecode(const char *pRecvBuf, const char *pKey, char *pRplyBuf);

	int SendPacket(SOCKET *pSocket, sockaddr_in* hostAddr, const char *pKey);

	int SendPacketVer2(SOCKET *pSocket, sockaddr_in* hostAddr, const char * ip,
		const int port, packetBufType *pBuf, const char *pKey);

	int recvPacket(SOCKET *pSocket,sockaddr_in *pClientAddr, const char *pKey, char *pResultBuf, int timeOut);
#ifdef __cplusplus
}
#endif

#endif
