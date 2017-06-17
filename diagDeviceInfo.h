/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   devInfo.h

  Description: Declaration of functions about devices information.

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-11-26, liguoliang      create file.
*******************************************************************************/

#ifndef _DEVINFO_H_
#define _DEVINFO_H_

/*******************************************************************************
  Function:    sysInfoInit
  Description: 8系列机型的系统初始化函数，函数实现链接8200a.lib库
  Input:       N/A
  Output:      N/A
  Return:      unsigned char
  Others:      N/A
*******************************************************************************/
unsigned char sysInfoInit();

/*******************************************************************************
  Function:    getSysSoftwareRevision
  Description: 获取8系列机型的软件版本信息，函数实现链接8200a.lib库
  Input:       N/A
  Output:      N/A
  Return:      返回软件版本字符串
  Others:      N/A
*******************************************************************************/
char *getSysSoftwareRevision(void);

/*******************************************************************************
  Function:    getSysHardwareRevision
  Description: 获取8系列机型的硬件版本信息，函数实现链接8200a.lib库
  Input:       N/A
  Output:      N/A
  Return:      返回硬件版本字符串
  Others:      N/A
*******************************************************************************/
char *getSysHardwareRevision(void);

/*******************************************************************************
  Function:    getSysIMEI
  Description: 获取8系列机型的IMEI信息，函数实现链接8200a.lib库
  Input:       N/A
  Output:      N/A
  Return:      返回IMEI字符串
  Others:      N/A
*******************************************************************************/
char *getSysIMEI(void);

#endif
