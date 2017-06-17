/*******************************************************************************
 Copyright (C), 2016 TP-LINK TECHNOLOGIES CO., LTD.

 File name:   TPWearableDeviceInfo.h

 Description: 通过TPWearable远程接口获取设备信息

 Author:      Liu Kai <liuKai_y0152@tp-link.net>

 History:
 ---------------------------------
 V1.0, 2016-01-27, Liu Kai      create file.
*******************************************************************************/

#ifndef _TPWDEVICEINFO_H_
#define _TPWDEVICEINFO_H_

#ifdef __cplusplus
extern "C"{
#endif

/*******************************************************************************
 Function:    StartTPWearableIperf
 Description: 通过命令控制TPWearable开启iperf
 Input:       pIP, test_in_dut
 return:      1 if success, 0 if fail
*******************************************************************************/
int StartTPWearableIperf(const char *test_in_dut, const char *pIP);

/*******************************************************************************
 Function:    GetTPWearableInfo
 Description: 通过命令请求获取TPWearable设备信息
 Input:       pIP, DUT端IP地址字符串
 Output:      存放设备信息Json到全局数组g_Json

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
int GetTPWearableInfo(const char *pIP);

/*******************************************************************************
 Function:    GetTPWearablexxx
 Description: 从全局数据中获取各类设备信息
 Input:       Buf, 存放信息的缓存地址
              BufLen, 缓存区大小

 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
int GetTPWearableIMEI(char *Buf);
int GetTPWearableMEID(char *Buf);
int GetTPWearableSoftVer(char *Buf);
int GetTPWearableHardVer(char *Buf);
int GetTPWearablePin(char *Buf);
int GetTPWearableMac(char *Buf);
int GetTPWearableWorkingMode(char *Buf);
int GetTPWearableModemStatus(char *Buf);
int GetTPWearableDeviceId(char *Buf);

/*******************************************************************************
 Function:    ResetTPWDevice
 Description: 通过命令恢复TPWearable设备出厂设置
 input:       pIP, DUT端IP地址字符串
 return:      1 if success, 0 if fail
 Author:      Liu Kai
 Date:        2016-01-27
*******************************************************************************/
int ResetTPWDevice(const char *pIP);

#ifdef __cplusplus
}
#endif

#endif //HTTPDEVICEINFO_H
