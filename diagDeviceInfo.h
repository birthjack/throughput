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
  Description: 8ϵ�л��͵�ϵͳ��ʼ������������ʵ������8200a.lib��
  Input:       N/A
  Output:      N/A
  Return:      unsigned char
  Others:      N/A
*******************************************************************************/
unsigned char sysInfoInit();

/*******************************************************************************
  Function:    getSysSoftwareRevision
  Description: ��ȡ8ϵ�л��͵�����汾��Ϣ������ʵ������8200a.lib��
  Input:       N/A
  Output:      N/A
  Return:      ��������汾�ַ���
  Others:      N/A
*******************************************************************************/
char *getSysSoftwareRevision(void);

/*******************************************************************************
  Function:    getSysHardwareRevision
  Description: ��ȡ8ϵ�л��͵�Ӳ���汾��Ϣ������ʵ������8200a.lib��
  Input:       N/A
  Output:      N/A
  Return:      ����Ӳ���汾�ַ���
  Others:      N/A
*******************************************************************************/
char *getSysHardwareRevision(void);

/*******************************************************************************
  Function:    getSysIMEI
  Description: ��ȡ8ϵ�л��͵�IMEI��Ϣ������ʵ������8200a.lib��
  Input:       N/A
  Output:      N/A
  Return:      ����IMEI�ַ���
  Others:      N/A
*******************************************************************************/
char *getSysIMEI(void);

#endif
