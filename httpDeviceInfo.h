#ifndef _HTTPDEVICEINFO_H_
#define _HTTPDEVICEINFO_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
  发送设置带宽的请求

  初始化成功，返回1；否则返回0.
*/
int sendWebData_PostBand(const char *pIP, const CString send_data, int nTimeout);

/*
  发送原始的Web请求数据

  初始化成功，返回1；否则返回0.
*/
int sendWebData(const char *pIP, const CString send_data, int nTimeout);

/*
  初始化基于HTTP协议，获取设备信息的方法。
  需传入router的IP及相应的密码，及超时的时间。默认的超时时间为10s。
  每次需要获得设备信息前，调用。

  初始化成功，返回1；否则返回0.
*/
int deviceInfoInit(const char *pIP, const char *pPassword, int nTimeout = 10);

/*******************************************************************************
  Function:    fixedBand
  Description: 固定信道，将带宽设置到40M
  Author:      renjian
  Date:        2015-01-05
*******************************************************************************/
int fixedBand(const char *pIP, int nTimeout, CString nonce, int &nCount, int channel, int bandwidth);

/*
  获取设备名称。
  pOut：保存设备名称的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceName(char *pOut, int nLen);

/*
  获取设备软件版本。
  pOut：保存设备软件版本的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceSysSoftVer(char *pOut, int nLen);

/*
  获取设备硬件版本。
  pOut：保存设备硬件版本的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceSysHardVer(char *pOut, int nLen);

/*
  获取设备MAC地址。
  pOut：保存设备MAC地址的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceMac(char *pOut, int nLen);

/*
  获取设备MEID。
  pOut：保存设备MEID的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceMeid(char *pOut, int nLen);

/*
  获取设备IMEI。
  pOut：保存设备IMEI的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceImei(char *pOut, int nLen);

/*
  获取设备Modem识别状态。
  pOut：保存设备Modem识别状态的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceModemStatus(char *pOut, int nLen);

/*
  获取设备工作模式。
  pOut：保存设备工作模式的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceWorkingMode(char *pOut, int nLen);

/*
  获取设备PIN码。
  pOut：保存设备PIN码的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDevicePin(char *pOut, int nLen);

/*
  获取设备nonce值。
  pOut：保存设备nonce值的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceNonce(char *pOut, int nLen);

/*
  获取设备devid。
  pOut：保存设备devid值的缓冲区。
  nLen：pOut缓冲区的大小。

  成功，返回1；否则返回0.
*/
int getDeviceId(char *pOut, int nLen);

/*
  判断web请求是否成功

  成功，返回1；否则返回0.
*/
int PageError();

/*
  解析返回的数据包是否包含需要登录的信息，以此判断cookie是否正确

  成功，返回1；否则返回0.
*/
int PassWordWrong();

/*
  解析返回数据得到CSRToken。

 解析成功，返回1；否则返回0.
*/
int getCSRFToken(char* csrf_token);

/*
  解析返回数据得到session_id。

 解析成功，返回1；否则返回0.
*/
int getSessionID(char* session_id);

/*
  解析返回的数据包获取7系列mifi产品的硬件版本号

 解析成功，返回1；否则返回0.
*/
int getDeviceInfo_7s(char *pOut, int nLen, int itemIndex);

/*
  解析返回的数据包获取8系列mifi产品的硬件版本号

 解析成功，返回1；否则返回0.
*/
int getDeviceInfo_8s(char *pOut, int nLen, int itemIndex);

/*
  解析返回的数据包获取TR961 CMCC产品的硬件版本号

 解析成功，返回1；否则返回0.
*/
int getDeviceInfo_lte_cmcc(const char *pKey, char *pOut, int nLen);

/*
 对TR961 CMCC进行认证处理

 解析成功，返回1；否则返回0.
*/
int authentication_lte_cmcc(const CString nonce, int &nCount, const int encrypt_code, CString &auth_field);

/*
  解析返回数据得到timeOut。

  解析成功，返回找到的值，否则返回默认值
*/
int getTimeOut(int nTimeout);

#ifdef __cplusplus
}
#endif

#endif //HTTPDEVICEINFO_H