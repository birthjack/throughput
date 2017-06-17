
/*********************************************************
* Copyright (c) TP-LINK Technologies CO., LTD.
* All rights reserved.
*文 件 名：   configure.h
*摘    要：   声明一些宏和结构定义
*作    者：   邓国伟
*创建日期：   2007-10-13
*********************************************************/


#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

#include <stdio.h>
#include "chrapi.h"
#include "diagDeviceInfo.h"
#include "device.h"

#include <ShellAPI.h>
#include <string>
#include <iostream>
#include <sstream>
#include <locale>
#include <stdio.h>
#include <tchar.h>
using namespace std;

#define  STOP_PING_DUT 1
#define  BEGIN_PING_DUT 2
#define  CLOSE_PING_DUT 3
#define  PAUSE_PING_DUT 4

#define  BEGIN_TEST 1
#define  STOP_TEST  0

#define BEGIN_CHECK_MIC 1
#define CHECK_MIC_SUCCESS 2
#define CHECK_MIC_FAIL 3

#define IPERF_STOP 0
#define IPERF_START_PC 1
#define IPERF_START_DUT 2
#define IPERF_SWITCH_PC 3
#define IPERF_SWITCH_DUT 4
#define IPERF_SWITCH_WRITE 5
#define IPERF_WORK_ERR 6

#define recordCount 20  //保存测试记录的最大条数

#define SCRIPTNAME             ".//script//Throughput.scr"
#define CONFIGURE_FILENAME     ".//config//Configure.ini"
#define JIXING                 ".//config//jixing.ini"

// [liguoliang start]
#define MAX_DEV_NUM 50 // 最大产品类型数量
// [liguoliang end]

typedef struct
{
	char region[24];
	char  channel;
}RegionAndChannel;

struct RUNSTATE
{
	short begin;
	short stop;
	short pause;
	short pingDUT;
	short checkMIC;
	short iperfStatus;
};

typedef struct
{
	int throughput_pass_value;
	short test_ver;
	short test_mac;
	short test_imei;
	short test_meid;
	short test_pin;
	short test_rst;
	short test_wan;
	short test_3gmodem;
	short test_format;
}ConfigThroughput;

typedef struct
{
	short has_sn;
	short has_mac;
	short has_meid;
	short has_imei;
	short has_pin;
}ConfigInfoWirte;


enum DEV_TYPES {
	DEVICE_7S,
	DEVICE_8S,
	DEVICE_ROUTER_AND_3G,
	DEVICE_ROUTER,
	DEVICE_LTE_MIFI,
	DEVICE_LTE_CMCC,
	DEVICE_LTE_MSM8909 = 7, // [renjian] 6 is used for Huawei V711 series in writeCodeManageTool
	DEVICE_TPWEARABLE,
	DEVICE_CARDV,
	DEVICE_TYPE_MAX
};

enum TEST_TYPES {
	CHARIOT_TEST,
	IPERF_TX_TEST,
	IPERF_RX_TEST,
	TEST_TYPE_MAX
};

// 所有这些全局变量的定义都在configure.cpp中
extern struct RUNSTATE runState;
extern int passtimes;
extern int failtimes;

extern char CON_model[];
extern int  CON_type;
extern int  CON_language;
extern char CON_DUTIP[];
extern int CON_WANTEST;
extern char CON_WANIP[];

extern char CON_DATANAME[];
extern char CON_DATAIP[];
extern char CON_username[];
extern char CON_password[];
extern char CON_DATASERVER[];
extern char CON_LISTNAME[];

extern char CON_testFilename[MAX_PATH];
extern char CON_remoteIP[];
extern int  CON_passAvg_24g;
extern int CON_passAvg_5g;

extern char CON_SoftVer[];
extern char CON_HardVer[];
extern int CON_PINcheck;
extern int CON_Reset;

extern char sn[15];
extern char StrResult[];
extern int  pairCount;
extern int product_total_num;
extern CHR_TEST_HANDLE test;
extern int DUTState;

extern double avgThroughput[];
extern void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING where);

extern char  nb[];
extern char  xb[];
extern char station_number[];
extern int passtimes;
extern int failtimes;
extern double passpro;

extern char result[];
extern CString jx;
extern char ap[];
extern CString DUTMAC;
extern CString pt;
extern CString ft;
extern CString pp;

extern HANDLE hCom;
extern int error_no;
extern int Flag;

extern CString DUTMACSHORT;
extern CString strSetPassword;
extern int product_total_num;
extern int CON_FORP;
extern int CON_AUDIO_RESULT;

extern CString testRecord[];
extern int counts;
extern int begRecordPos;

// [liguoliang start]
extern DevModel allDev[];
extern DevInfo productWebInfo;
extern DevInfo productDBInfo;
extern ConfigThroughput product_test;
extern ConfigInfoWirte prodcut_info_write;

extern char CON_DUTUSER[];
extern char CON_DUTPASSWORD[];

extern int CON_EDITCONFIG;
extern int CON_COMPATIBLE_OLD_SW;
extern int CON_SET_NEW_PASSWORD;
extern int CON_CurSel;
extern int CON_DevType;

extern int CON_RESET_TIME;
extern int CON_DBConnect;
extern int CON_CHECKVER;
extern int CON_FormatDisk;
extern int CON_3GModem;
extern int CON_THROUGH_TEST;
extern int CON_AUDIO_TEST;

extern int WIFI_CONTROL;
extern int CON_DUAL_BAND_CONTROL;
extern unsigned int dev_model_id;
extern unsigned long product_id;

extern int DUT_CONNECT;
extern HANDLE connDUTThread;
extern HANDLE wifiThread;
// [liguoliang end]

// [renjian start]
extern int CON_HT40;
extern HANDLE hMutex_mic;
extern HANDLE hMutex_iperf;
// [renjian end]

// [renjian start] MIC support
extern char CON_LINE[];
extern char CON_EQUIP[];
extern char CON_CHECK_STATION[];
extern int CON_MES_ENABLE;
extern int CON_ROUTE_CHECK_ENABLE;
extern int CON_ROUTE_CHECK_RESULT;
extern int MIC_CONNECT;
extern int DEVID_GET;
extern HANDLE connMICThread;
extern char CON_MIC_SERVER[];
// [renjian end]

// 版本信息检查(只用于非MiFi项目)
extern char CON_CHECK_HARDWARE[];
extern char CON_CHECK_SOFTWARE[];

// [renjian start] iperf support
extern int CON_TEST_TYPE;
extern int CON_TEST_TIME;
extern char CON_iperf_tx_pc[];
extern char CON_iperf_tx_dut[];
extern char CON_iperf_rx_pc[];
extern char CON_iperf_rx_dut[];
extern HANDLE iperfDutThread;
extern HANDLE iperfPcThread;
extern HANDLE audioResultThread;
extern HANDLE myTimeOutThread;
// [renjian end]


enum DUT_state
{
	DUT_STATE_UNSTARTUP = 0, //DUT未启动
	DUT_STATE_STARTUP,       //已启动
	DUT_STATE_TESTING,       //正在测试
	DUT_STATE_TESTED,        //已测试完
	DUT_STATE_CONFIGUREING,     //正在配置DUT
	DUT_STATE_CONFIGURED,      //已配置完
	DUT_STATE_RESTOREING,
	DUT_STATE_RESTORED,
	DUT_STATE_RESTARTUP,      //重启中
	DUT_STATE_STOP            //停止
};

/*******************************************************************************
  Function:    initialGlobalConfigure
  Description: 初始化软件的所有全局变量
  Author:      liguoliang
  Date:        2013-12-06
*******************************************************************************/
void initialGlobalConfigure();

/*******************************************************************************
  Function:    LoadDLLFunctions
  Description: 载入相关dll动态链接库，调用其中的函数进行相应的操作
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
BOOLEAN LoadDLLFunctions();

/*******************************************************************************
  Function:    initialCOM
  Description: 初始化hCom串口句柄
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN initialCOM();

/*******************************************************************************
  Function:    readDevInfoFromString
  Description: 从字符串中读取相关列的值
  Author:      liguoliang
  Date:        2014-01-08
*******************************************************************************/
int readDevInfoFromString(char* d_str, DevModel* dev);

/*******************************************************************************
  Function:    findThrIperfResult
  Description: 解析iperf返回结果，得到性能数据
  Author:      renjian
  Date:        2016-04-15
*******************************************************************************/
double findThrIperfResult(char *l_shellResultBuff);

#endif