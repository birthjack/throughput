#ifndef _HTTPDEVICEINFO_H_
#define _HTTPDEVICEINFO_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
  �������ô��������

  ��ʼ���ɹ�������1�����򷵻�0.
*/
int sendWebData_PostBand(const char *pIP, const CString send_data, int nTimeout);

/*
  ����ԭʼ��Web��������

  ��ʼ���ɹ�������1�����򷵻�0.
*/
int sendWebData(const char *pIP, const CString send_data, int nTimeout);

/*
  ��ʼ������HTTPЭ�飬��ȡ�豸��Ϣ�ķ�����
  �贫��router��IP����Ӧ�����룬����ʱ��ʱ�䡣Ĭ�ϵĳ�ʱʱ��Ϊ10s��
  ÿ����Ҫ����豸��Ϣǰ�����á�

  ��ʼ���ɹ�������1�����򷵻�0.
*/
int deviceInfoInit(const char *pIP, const char *pPassword, int nTimeout = 10);

/*******************************************************************************
  Function:    fixedBand
  Description: �̶��ŵ������������õ�40M
  Author:      renjian
  Date:        2015-01-05
*******************************************************************************/
int fixedBand(const char *pIP, int nTimeout, CString nonce, int &nCount, int channel, int bandwidth);

/*
  ��ȡ�豸���ơ�
  pOut�������豸���ƵĻ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceName(char *pOut, int nLen);

/*
  ��ȡ�豸����汾��
  pOut�������豸����汾�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceSysSoftVer(char *pOut, int nLen);

/*
  ��ȡ�豸Ӳ���汾��
  pOut�������豸Ӳ���汾�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceSysHardVer(char *pOut, int nLen);

/*
  ��ȡ�豸MAC��ַ��
  pOut�������豸MAC��ַ�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceMac(char *pOut, int nLen);

/*
  ��ȡ�豸MEID��
  pOut�������豸MEID�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceMeid(char *pOut, int nLen);

/*
  ��ȡ�豸IMEI��
  pOut�������豸IMEI�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceImei(char *pOut, int nLen);

/*
  ��ȡ�豸Modemʶ��״̬��
  pOut�������豸Modemʶ��״̬�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceModemStatus(char *pOut, int nLen);

/*
  ��ȡ�豸����ģʽ��
  pOut�������豸����ģʽ�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceWorkingMode(char *pOut, int nLen);

/*
  ��ȡ�豸PIN�롣
  pOut�������豸PIN��Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDevicePin(char *pOut, int nLen);

/*
  ��ȡ�豸nonceֵ��
  pOut�������豸nonceֵ�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceNonce(char *pOut, int nLen);

/*
  ��ȡ�豸devid��
  pOut�������豸devidֵ�Ļ�������
  nLen��pOut�������Ĵ�С��

  �ɹ�������1�����򷵻�0.
*/
int getDeviceId(char *pOut, int nLen);

/*
  �ж�web�����Ƿ�ɹ�

  �ɹ�������1�����򷵻�0.
*/
int PageError();

/*
  �������ص����ݰ��Ƿ������Ҫ��¼����Ϣ���Դ��ж�cookie�Ƿ���ȷ

  �ɹ�������1�����򷵻�0.
*/
int PassWordWrong();

/*
  �����������ݵõ�CSRToken��

 �����ɹ�������1�����򷵻�0.
*/
int getCSRFToken(char* csrf_token);

/*
  �����������ݵõ�session_id��

 �����ɹ�������1�����򷵻�0.
*/
int getSessionID(char* session_id);

/*
  �������ص����ݰ���ȡ7ϵ��mifi��Ʒ��Ӳ���汾��

 �����ɹ�������1�����򷵻�0.
*/
int getDeviceInfo_7s(char *pOut, int nLen, int itemIndex);

/*
  �������ص����ݰ���ȡ8ϵ��mifi��Ʒ��Ӳ���汾��

 �����ɹ�������1�����򷵻�0.
*/
int getDeviceInfo_8s(char *pOut, int nLen, int itemIndex);

/*
  �������ص����ݰ���ȡTR961 CMCC��Ʒ��Ӳ���汾��

 �����ɹ�������1�����򷵻�0.
*/
int getDeviceInfo_lte_cmcc(const char *pKey, char *pOut, int nLen);

/*
 ��TR961 CMCC������֤����

 �����ɹ�������1�����򷵻�0.
*/
int authentication_lte_cmcc(const CString nonce, int &nCount, const int encrypt_code, CString &auth_field);

/*
  �����������ݵõ�timeOut��

  �����ɹ��������ҵ���ֵ�����򷵻�Ĭ��ֵ
*/
int getTimeOut(int nTimeout);

#ifdef __cplusplus
}
#endif

#endif //HTTPDEVICEINFO_H