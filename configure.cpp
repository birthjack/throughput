/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   configure.cpp

  Description: 定义整个软件的全局变量

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-02, liguoliang      create file.
*******************************************************************************/
#include <string.h>
#include "StdAfx.h"
#include "configure.h"
#include "icmp.h"
#include "fmifs.h"

struct RUNSTATE runState;

//定义界面上的配置为全局变量，加上前缀"CON_"
char CON_model[MAX_DEVICE_NAME_LEN];                 // 机型名称
int  CON_type;                      // 0 - router, 1 - AP, 2 - NIC
int  CON_language;                  // 0 - 中文, 1 - 英文
char CON_DUTIP[16];                 // DUT的LAN端IP
char CON_WANIP[16];                 // DUT的WAN端IP

char CON_DATANAME[32];              // 数据库名称
char CON_DATAIP[16];                // 数据库服务器IP
char CON_username[32];              // 数据库用户名
char CON_password[16];              // 数据库密码
char CON_DATASERVER[32];            // 数据库服务器

char CON_LISTNAME[16];              // 记录测试结果的表名称


// [liguoliang start]
char CON_DUTUSER[32];               // 设备默认登录名
char CON_DUTPASSWORD[32];           // 设备默认登录密码

int CON_EDITCONFIG;                 // 是否可以更改配置测试
int CON_COMPATIBLE_OLD_SW;          // 是否兼容老版本软件(获取信息方式不一样)
int CON_SET_NEW_PASSWORD;           // 是否修改管理页面登陆密码

DevModel allDev[MAX_DEV_NUM];       // 从配置文件中读取的机型信息
DevInfo productWebInfo;             // 当前测试产品的Web请求相关信息
DevInfo productDBInfo;              // 当前测试产品的数据库中相关信息
ConfigThroughput product_test;      // 在进行吞吐量测试前进行的配置测试
ConfigInfoWirte prodcut_info_write; // 该机型对应的写码工具配置

int CON_CurSel;                     // 默认选择的设备
int CON_DevType;                    // 产品的类型
int CON_DBConnect;                  // 是否连接数据库
int CON_CHECKVER;                   // 是否检查版本信息和MAC地址
int CON_FormatDisk;                 // 是否格式化用户盘，1-yes，0-no
int CON_3GModem;                    // 是否进行3G Modem适配判断，1-yes，0-no
int CON_WANTEST;                    // 是否进行WAN口测试
int CON_THROUGH_TEST;               // 是否进行性能测试
int CON_AUDIO_TEST;                 // 是否进行音频测试

int CON_RESET_TIME;                 // 软件复位等待时间

unsigned int dev_model_id;          // 从数据库中查询到的当前设备类型的id
unsigned long product_id;           // 从数据库中查询到的当前产品的id

int WIFI_CONTROL=0;                 // 是否显示“开启WiFi”按钮
int CON_DUAL_BAND_CONTROL=0;        // 是否测试双频
int product_total_num = 0;          // 总得机型数量
int DUT_CONNECT = 0;                // 表示设备连接状态
HANDLE connDUTThread;               // 检测设备线程句柄
HANDLE wifiThread;                  // 打开设备wifi线程句柄
// [liguoliang end]
HANDLE connMICThread;               // 检测MIC服务器线程句柄
HANDLE hMutex_mic;                      // mic线程锁
HANDLE hMutex_iperf;					// iperf 线程锁
HANDLE iperfDutThread;              // iperf dut 端线程
HANDLE iperfPcThread;               // iperf pc 端线程
HANDLE audioResultThread;           // 音频测试工具获取结果线程
HANDLE myTimeOutThread;             // 超时线程

// [renjian start]
int CON_HT40 = 0;                   // 设置40M带宽标志位
// [renjian end]

char CON_testFilename[MAX_PATH];    // 吞吐量测试文件名
char CON_remoteIP[16];              // 对端IP地址
int  CON_passAvg_24g;                   // 吞吐量合格值2.4G
int  CON_passAvg_5g;                 // 吞吐量合格值5G
char CON_SoftVer[50];               // 保存从数据库中获取的软件版本
char CON_HardVer[50];               // 保存从数据库中获取的硬件版本

int CON_PINcheck;                   // check PIN  1:yes,0:no
int CON_Reset;                      // reset DUT  1:yes,0:no

char sn[15];                        // SN号
char StrResult[20];                 // 测试结论
int  pairCount;                     // pair数目
int DUTState;                       // DUT状态
double avgThroughput[2];            // 保存两次测试的平均吞吐量值
char  nb[10];                        // 测试员工号
char  xb[15];	                    // 线别
char station_number[15];            // 工位

int passtimes=0;                    // 合格次数
int failtimes=0;                    // 不合格次数
double passpro=0.0;                 // 一次测试直通率
CString pt,ft,pp;                   // 分别保存上述三个值的字符串

char result[10000];                 // 缓存获得的数据
CString jx;                         // 机型名称
char ap[100]={0};                   // 机型信息
CString DUTMAC;                     // 通过PING获得DUT的MAC地址

HANDLE hCom;                        // 准备打开的串口的句柄
int error_no=0;                     // 测试过程中的错误码
int Flag=0;                         // 标记是否已经存在该MAC所对应的设备的信息在测试记录里

CString DUTMACSHORT;                // 将MAC显示中的-去除

int CON_FORP = 2;                   // 是否合格 1合格0不合格2待定
int CON_AUDIO_RESULT = 2;           // 音频测试结果 1合格 0不合格 2待定
CString testRecord[recordCount];    // 测试记录
int counts = 0;                     // 已经保存的测试记录数
int begRecordPos = 0;               // 循环数组中开始的那个下标

int CON_TEST_TYPE = 0;
int CON_TEST_TIME = 20;
char CON_iperf_tx_pc[64];
char CON_iperf_tx_dut[64];
char CON_iperf_rx_pc[64];
char CON_iperf_rx_dut[64];

// MIC所用数据
char CON_LINE[11];
char CON_EQUIP[21];
char CON_CHECK_STATION[16];
char CON_MIC_SERVER[32];
int CON_MES_ENABLE;
int CON_ROUTE_CHECK_ENABLE;
int CON_ROUTE_CHECK_RESULT = FALSE;
int MIC_CONNECT = FALSE;
int DEVID_GET = FALSE;

// 版本信息检查(用于非MiFi项目)
char CON_CHECK_HARDWARE[32];
char CON_CHECK_SOFTWARE[32];


// 通过icmp.dll获得的函数
pfnHV pIcmpCreateFile;
pfnBH pIcmpCloseHandle;
pfnDHDPWPipPDD pIcmpSendEcho;

// 通过fmifs.dll获得的函数
PFORMATEX FormatEx;

HMODULE fmisfDll = NULL;            // 指向fmisf.dll
HMODULE icmpDll = NULL;             // 指向icmp.dll
HMODULE commonlibDll = NULL;
HMODULE DevIDGenDll = NULL;
HMODULE WSDataTransDll = NULL;

extern CHR_TEST_HANDLE test;
extern void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING where);


/*******************************************************************************
  Function:    initialGlobalConfigure
  Description: 初始化软件的所有全局变量
  Author:      liguoliang
  Date:        2013-12-06
*******************************************************************************/
void initialGlobalConfigure()
{
	product_total_num = GetPrivateProfileInt("机型信息", "机型总数", 0, JIXING);
	CON_CurSel = GetPrivateProfileInt("其他", "默认选择", 0, JIXING);
	CON_DevType = GetPrivateProfileInt("其他", "测试类型", 0, JIXING);

	CON_COMPATIBLE_OLD_SW = GetPrivateProfileInt("其他", "兼容老软件", 0, CONFIGURE_FILENAME);
	CON_SET_NEW_PASSWORD = GetPrivateProfileInt("其他", "修改登录密码", 1, CONFIGURE_FILENAME);
	CON_EDITCONFIG = GetPrivateProfileInt("其他", "是否可配置", 1, CONFIGURE_FILENAME);
	WIFI_CONTROL = GetPrivateProfileInt("其他", "控制WIFI", 0, CONFIGURE_FILENAME);
	CON_DUAL_BAND_CONTROL = GetPrivateProfileInt("其他", "是否测试双频", 0, CONFIGURE_FILENAME);

	// [liguoliang start]
	/*strcpy(CON_DUTIP, products[CON_CurSel].lan_ip);*/
	GetPrivateProfileString("DUT参数", "默认IP", "192.168.1.1", CON_DUTIP, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT参数", "默认user", "admin", CON_DUTUSER, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT参数", "默认password", "admin", CON_DUTPASSWORD, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT参数", "WANIP", "131.55.66.134", CON_WANIP, 16, CONFIGURE_FILENAME);
	CON_DBConnect = GetPrivateProfileInt("其他", "连接数据库", 1, CONFIGURE_FILENAME);
	// [liguoliang end]

	GetPrivateProfileString("数据库", "数据库IP", "192.168.2.1", CON_DATAIP, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("数据库", "用户名", "admin", CON_username, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("数据库", "密码", "admin", CON_password, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("数据库", "数据库名称", "test", CON_DATANAME, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("数据库", "数据库服务器", "无线测试", CON_DATASERVER, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("数据库", "表名称", "nx3fc", CON_LISTNAME, 16, CONFIGURE_FILENAME);

	GetPrivateProfileString("吞吐量测试配置", "测试文件名", "LastTestFile.tst", CON_testFilename, MAX_PATH, CONFIGURE_FILENAME);
	GetPrivateProfileString("吞吐量测试配置", "对端IP", "192.168.1.2", CON_remoteIP, 16, CONFIGURE_FILENAME);
	CON_passAvg_24g = GetPrivateProfileInt("吞吐量测试配置", "吞吐量_24g", 10, CONFIGURE_FILENAME);
	CON_passAvg_5g = GetPrivateProfileInt("吞吐量测试配置", "吞吐量_5g", 10, CONFIGURE_FILENAME);
	CON_RESET_TIME = GetPrivateProfileInt("吞吐量测试配置", "复位等待时间", 3, CONFIGURE_FILENAME)*1000;
	CON_TEST_TYPE = GetPrivateProfileInt("吞吐量测试配置", "测试拓扑选择", 0, CONFIGURE_FILENAME);
	CON_TEST_TIME = GetPrivateProfileInt("吞吐量测试配置", "测试时间", 20, CONFIGURE_FILENAME);
	GetPrivateProfileString("吞吐量测试配置", "IPERF_TX_PC", "iperf -v", CON_iperf_tx_pc, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("吞吐量测试配置", "IPERF_TX_DUT", "iperf -v", CON_iperf_tx_dut, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("吞吐量测试配置", "IPERF_RX_PC", "iperf -v", CON_iperf_rx_pc, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("吞吐量测试配置", "IPERF_RX_DUT", "iperf -v", CON_iperf_rx_dut, 64, CONFIGURE_FILENAME);

	passtimes = GetPrivateProfileInt("测试统计", "测试合格次数", 10, CONFIGURE_FILENAME);
	failtimes = GetPrivateProfileInt("测试统计", "测试不合格次数", 10, CONFIGURE_FILENAME);

	char passpro_string[10];
	GetPrivateProfileString("测试统计","一次测试直v通率", "0.0", passpro_string, 10, CONFIGURE_FILENAME);
	passpro = atof(passpro_string);

	// [renjian start] MIC
	GetPrivateProfileString("MIC", "生产线编号", "4FD-2", CON_LINE, 11, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "测试设备名称", "1", CON_EQUIP, 21, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "上一测试站编码", "T10", CON_CHECK_STATION, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "MIC服务器地址", "mictest.rd.tp-link.net", CON_MIC_SERVER, 32, CONFIGURE_FILENAME);

	CON_MES_ENABLE = GetPrivateProfileInt("MIC", "MES使能", 0, CONFIGURE_FILENAME);
	CON_ROUTE_CHECK_ENABLE = GetPrivateProfileInt("MIC", "流程检查使能", 0, CONFIGURE_FILENAME);
	// [renjian end]

	// 版本信息检查澹(用于非MIFI项目)
	GetPrivateProfileString("版本检查", "硬件版本", "N/A", CON_CHECK_HARDWARE, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("版本检查", "软件版本", "N/A", CON_CHECK_SOFTWARE, 32, CONFIGURE_FILENAME);
}

/*******************************************************************************
  Function:    LoadDLLFunctions
  Description: 载入相关dll动态链接库，调用其中的函数进行相应的操作
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
BOOLEAN LoadDLLFunctions()
{
	fmisfDll = LoadLibrary( "fmifs.dll" ); // 载入fmifs.dll库,进行格式化操作
	icmpDll = LoadLibrary("icmp.dll"); // 载入icmp.dll库,进行ping操作

	if (fmisfDll == NULL || icmpDll == NULL) {
		return FALSE;
	}

	// 找到执行格式化操作的FormatEx函数地址
	FormatEx = (PFORMATEX) GetProcAddress( fmisfDll,"FormatEx" );

	// 找到执行发送arp包的相关函数地址
	pIcmpCreateFile = (pfnHV)GetProcAddress(icmpDll,  "IcmpCreateFile");
	pIcmpCloseHandle = (pfnBH)GetProcAddress(icmpDll, "IcmpCloseHandle");
	pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(icmpDll, "IcmpSendEcho");

	if ((pIcmpCreateFile == NULL) || (pIcmpCloseHandle == NULL) ||
		(pIcmpSendEcho == NULL) || (FormatEx == NULL)) {
			return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
  Function:    initialCOM
  Description: 初始化hCom串口句柄
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN initialCOM()
{
	//初始化串口功能
	hCom=CreateFile("COM1",               // port name
		GENERIC_READ | GENERIC_WRITE,     // allow r/w access
		0,                                // always no sharing
		0,                                // no security atributes for file
		OPEN_EXISTING,                    // always open existing
		//FILE_FLAG_OVERLAPPED,            // overlapped operation
		FILE_FLAG_NO_BUFFERING,            // non-overlapped operation
		0);                               // always no file template

	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("create handle err!");
		return FALSE;
	}

	//初始化dcb并绑定到hCom中
	DCB dcb;
	// port configuration
	FillMemory (&dcb, sizeof(dcb),0);
	dcb.DCBlength = sizeof(dcb);
	if (!BuildCommDCB("9600,n,8,1", &dcb))
	{
		printf("BuildCommDCB err");
		return FALSE;
	}

	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	if (!SetCommState(hCom, &dcb))
	{
		printf("SetCommState err!");
		return FALSE;
	}

	COMMTIMEOUTS timeOver ; //定义超时结构，并填写该结构
	memset(&timeOver,0,sizeof(timeOver));
	timeOver.ReadTotalTimeoutMultiplier=10000;
	timeOver.ReadTotalTimeoutConstant=10000;
	if (!SetCommTimeouts(hCom,&timeOver))
	{
		printf("SetCommTimeouts err!");
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
  Function:    readDevInfoFromString
  Description: 从字符串中读取相关列的值
  Author:      liguoliang
  Date:        2014-01-08
*******************************************************************************/
int readDevInfoFromString(char* d_str, DevModel* dev)
{
	if (NULL == d_str || NULL == dev)
	{
		return FALSE;
	}
	char temp[MAX_DEVINFO_SIZE];
	char items[DEVINFO_ITEM][MAX_DEVICE_NAME_LEN];

	strcpy(temp, d_str);
	char *p; p = strtok(temp, ",");

	for (int i = 0; i<DEVINFO_ITEM;i++)
	{
		CString s = p;
		s = s.TrimLeft();
		s = s.TrimRight();
		strcpy(items[i],(LPSTR)(LPCTSTR)s);
		p = strtok(NULL, ",");
	}
	strcpy(dev->product_name, items[0]);
	dev->close_wifi = atoi(items[1]);
	return TRUE;
}

double findThrIperfResult(char *l_shellResultBuff)
{
	char *pos_first = NULL;
	char *pos_end = NULL;
	const char *findstr_first =  "MBytes  ";
	const char *findstr_end = "Mbits";
	char buf[256]= {0};

	pos_first = strstr(l_shellResultBuff, findstr_first);
	pos_end = strstr(l_shellResultBuff, findstr_end);
	if (pos_first && pos_end)
	{
		strncpy(buf, pos_first + strlen(findstr_first), (pos_end - pos_first - strlen(findstr_first))/sizeof(char) + 1 - 2);
		buf[(pos_end - pos_first - strlen(findstr_first))/sizeof(char) + 1 - 2] = '\0';
		//char *tmp = ltrim_lc(rtrim_lc(buf));
		return atof(buf);
	}
	else
	{
		return 0;
	}

}

