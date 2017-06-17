/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   icmp.h

  Description: 定义和icmp.dll相关的载入、函数获取等变量和函数

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-02, liguoliang      create file.
*******************************************************************************/

#ifndef _ICMP_H_
#define _ICMP_H_

#include <winsock.h>
#include <windowsx.h>

typedef struct {
	unsigned char Ttl;                         // Time To Live
	unsigned char Tos;                         // Type Of Service
	unsigned char Flags;                       // IP header flags
	unsigned char OptionsSize;                 // Size in bytes of options data
	unsigned char *OptionsData;                // Pointer to options data
} ICMP_OPTION_INFORMATION, * PICMP_OPTION_INFORMATION;

typedef struct {
	DWORD Address;                             // Replying address
	unsigned long  Status;                     // Reply status
	unsigned long  RoundTripTime;              // RTT in milliseconds
	unsigned short DataSize;                   // Echo data size
	unsigned short Reserved;                   // Reserved for system use
	void *Data;                                // Pointer to the echo data
	ICMP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, * PIP_ECHO_REPLY;

typedef HANDLE (WINAPI* pfnHV)(VOID);
typedef BOOL (WINAPI* pfnBH)(HANDLE);
typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
	PICMP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);

extern pfnHV pIcmpCreateFile;
extern pfnBH pIcmpCloseHandle;
extern pfnDHDPWPipPDD pIcmpSendEcho;
extern HMODULE icmpDll;

/*******************************************************************************
  Function:    myPing
  Description: 通过调用icmp.dll的函数来发送ping数据判断是否连接上指定ip
*******************************************************************************/
int myPing(char IPAddr[16]);

#endif