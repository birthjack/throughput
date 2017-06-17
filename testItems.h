/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   testItems.h

  Description: 声明测试项实现的函数，将原来在同一个文件中定义的函数分离出来

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-05, liguoliang      create file.
*******************************************************************************/

#ifndef _TESTITEMS_H_
#define _TESTITEMS_H_

//#define MATCH_VER 1
//#define DISCONNECT_DUT -1
//#define UNKNOWN_PASSWORD -2
//#define UNMATCH_SW_VER -3
//#define UNMATCH_HW_VER -4
//#define UNMATCH_MAC -5
//#define UNMATCH_PIN -6

// 定义在调用获取设备信息的时候返回的错误代码
typedef enum{
	MATCH_VER = 1,
	DISCONNECT_DUT = -1,
	UNKNOWN_PASSWORD = -2,
	UNMATCH_SW_VER = -3,
	UNMATCH_HW_VER = -4,
	UNMATCH_MAC = -5,
	UNMATCH_PIN = -6
}GET_VER_ERROR;

#define  TEST_NUM 1  // 循环测试的次数


// 定义检测项不通过的错误代码
typedef enum{
	PASS_NOERROR = 0,
	SOFTWARE_UNMATCH,
	HARDWARE_UNMATCH,
	PIN_ERROR,
	MAC_ERROR,
	WAN_ERROR
}FAIL_ERROR;

DWORD WINAPI timeOutThread(LPVOID pParam);
DWORD WINAPI audioResult(LPVOID pParam);
DWORD WINAPI iperfPCThread(LPVOID pParam);
DWORD WINAPI iperfDUTThread(LPVOID pParam);
/*********************************************************
*函 数 名：   throughputTest
*摘    要：   测试吞吐量，若第一次不合格则再测一次，
*             在该函数中，主要是调用beginThroughputTest()
*作    者：   邓国伟
*创建日期：   2007-10-15
*********************************************************/
DWORD WINAPI throughputTest(LPVOID pParam);

/*******************************************************************************
  Function:    openWiFiThread
  Description: 一直开启WiFi知道停止为止
  Author:      liguoliang
  Date:        2013-12-25
*******************************************************************************/
DWORD WINAPI openWiFiThread(LPVOID pParam);

bool pingWAN(char IPaddr[16]);


/*******************************************************************************
  Function:    getMACInfo
  Description: 通过发送ARP包获取MAC地址
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
int getMACInfo(char *ip, char *resultBuf);

/*********************************************************
*函 数 名：   insertTestRecord
*摘    要：   将测试记录添加到有recordCount个元素的循环数组中
*作    者：   邓国伟
*创建日期：   2007-10-22
*********************************************************/
void insertTestRecord(double testValue, int throughput_threshold);

/*******************************************************************************
  Function:    formatDisk
  Description: 针对7些列有存储盘的的机型，格式化存储盘
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int formatDisk();

/*******************************************************************************
  Function:    AddError
  Description: 分离CMyDlg中的AddError函数成单独的函数
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
void AddError(int er);

/*******************************************************************************
  Function:    connectDev
  Description: 连接DUT和对方主机
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN connectDev();

/*******************************************************************************
  Function:    saveFileLog
  Description: 保存吞吐量测试结果到log文件中
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN saveFileLog(CString avg, CTime time);

/*******************************************************************************
  Function:    testWAN
  Description: 测试WAN口
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testWAN();

/*******************************************************************************
  Function:    testVersion
  Description: 检查软硬件版本和网页显示的MAC地址是否匹配等工作
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testVersion();

/*******************************************************************************
  Function:    resetDUT
  Description: 软件复位
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int resetDUT();

/*******************************************************************************
  Function:    testPIN
  Description: 检查PIN码
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int testPIN();

/*******************************************************************************
  Function:    throughputPass
  Description: 记录吞吐量合格
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputPass(CString throughput_value);

/*******************************************************************************
  Function:    throughputNoPass
  Description: 记录吞吐量不合格
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputNoPass(CString throughput_value);

/*******************************************************************************
  Function:    formatDUT
  Description: 格式化用户盘
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int formatDUT();

/*******************************************************************************
  Function:    test3GModem
  Description: 检查3G Modem是否适配
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int test3GModem();

/*******************************************************************************
  Function:    testMIC
  Description: 检查MIC状态
  Author:      renjian
  Date:        2015-10-29
*******************************************************************************/
int testMIC();

/*******************************************************************************
  Function:    testType
  Description: 检查测试项勾选状态
  Author:      renjian
  Date:        2016-10-31
*******************************************************************************/
int testType();

/*******************************************************************************
  Function:    getDBProductInfo
  Description: 从数据库中检查MAC地址及其他信息写入情况
  Author:      liguoliang
  Date:        2013-12-18
*******************************************************************************/
int getDBProductInfo();

/*******************************************************************************
  Function:    checkExistDBLog
  Description: 根据MAC查询是否已经存在对应的产品的测试记录
  Author:      liguoliang
  Date:        2013-12-19
*******************************************************************************/
int checkExistDBLog();

/*******************************************************************************
  Function:    changeDBProductStatus
  Description: 修改数据库中Product表的代表测试结果的p_status字段的值
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBProductStatus(int success_or_failed);

/*******************************************************************************
  Function:    deleteDBPassLog
  Description: 修改原来测试成功的记录并更改Product表的status字段
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBPassLog(ULONG tl_id);

/*******************************************************************************
  Function:    getDevInfo
  Description: 获取设备信息并显示在界面上
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int getDevInfo();

/*******************************************************************************
  Function:    iperfTest
  Description: iperf工具测试接口
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void iperfTest();

/*******************************************************************************
  Function:    chariotTest
  Description: chariot工具测试接口
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void chariotTest();

/*******************************************************************************
  Function:    autioTest
  Description: 音频测试工具接口
  Author:      renjian
  Date:        2016-05-24
*******************************************************************************/
void audioTest();

/*******************************************************************************
  Function:    audioListen
  Description: 音频播放接口
  Author:      renjian
  Date:        2016-05-31
*******************************************************************************/
void audioListen();

/*******************************************************************************
  Function:    fixBandTo40
  Description: 设置固定信道，并将信道带宽设置为40M
  Author:      renjian
  Date:        2015-01-07
*******************************************************************************/
int fixBandTo40(int channel, int bandwidth);

/*******************************************************************************
  Function:    connectDUTThread
  Description: 连接设备线程，该线程会在选择机型后一直运行检测设备
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
DWORD WINAPI connectDUTThread(LPVOID pParam);

/*******************************************************************************
  Function:    connectMICThread
  Description: 连接MIC服务器线程，该线程会在选择机型后一直运行检测MIC服务器
  Author:      renjian
  Date:        2015-10-20
*******************************************************************************/
DWORD WINAPI connectMICThread(LPVOID pParam);

/*******************************************************************************
  Function:    mac_compare
  Description: 自己定义的mac比较方法
  Author:      liguoliang
  Date:        2014-09-05
*******************************************************************************/
int mac_compare(char* mac1, char* mac2);

/*******************************************************************************
  Function:    setField
  Description: 作业站基础数据赋值
  Author:      renjian
  Date:        2015-10-09
*******************************************************************************/
int setField(char buffer[], int bufsiz, const char * valstr);

/*******************************************************************************
  Function:    uploadData
  Description: 数据上传到MIC服务器
  Author:      renjian
  Date:        2015-10-09
*******************************************************************************/
int uploadData(CString avgThroughput, char starttime[], char endtime[], int throughput_threshold);

#endif