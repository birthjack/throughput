/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   device.h

  Description: 定义设备的父类和目前的子类

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-09, liguoliang      create file.
*******************************************************************************/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "diagDeviceInfo.h"

#define MAX_DEVINFO_SIZE 1024 // 配置文件中产品信息的最大字符数
#define DEVINFO_ITEM 18       // 配置文件中产品信息的参数个数

#define MAX_DEVICE_NAME_LEN 64

typedef struct DEVECEMODEL
{
	char product_name[MAX_DEVICE_NAME_LEN]; // 产品名称
	unsigned short close_wifi;   // 是否自动关闭wifi, 只针对路由器方案有效
}DevModel;

// 设备信息结构体
typedef struct DEVINFO
{
	char product_sw_ver[128];  // 产品软件版本号
	char product_hw_ver[128];  // 产品硬件版本号
	char product_name[64];     // 产品名称
	char product_mac[20];      // 产品MAC地址
	char product_sn[16];       // 产品的SN号
	char product_imei[16];     // 产品的IMEI号
	char product_meid[16];     // 产品的MEID号
	char product_pin[10];      // 产品的PIN码
	char product_work_mode[16]; // 产品的工作模式
	char product_modem_status[16];// 产品的modem识别状态
	char product_devid[41];    //产品devid序号
}DevInfo;


enum WiFiBand {
	BAND_INVALID,
	BAND_2_GHZ = 2,
	BAND_5_GHZ = 5
};

/*
  表示产品类型的父类
 */
class Device
{
protected:
	char imei[16];         // IMEI号
	char meid[16];         // MEID号
	char mac[18];          // MAC地址
	char sw_version[64];   // 软件版本号
	char hw_version[64];   // 硬件版本号
	char pin[16];          //PIN码
	char working_mode[16]; // 工作模式
	char modem_status[16]; // modem识别结果
	char devid[41]; //devid序号

	char buff[1024];      // socket接受数据缓存

	enum WiFiBand band;	// WiFi 频段

    int mac_suffix;  //SSID 需要MAC的位数

public:
	Device(){this->band = BAND_2_GHZ; this->mac_suffix = 6;}
	virtual ~Device(){}

	DevInfo info;
	int initial;           // 生成对象时初始化工作如果失败置0,成功置1

	// 以下几个虚函数目前针对移动互联网产品线的所有产品都使用统一的方法实现
	virtual char* getIMEI(); // 获取IMEI号
	virtual char* getMEID(); // 获取MEID号
	virtual char* getSoftWareVersion(); // 获取软件版本号
	virtual char* getHardWareVersion(); // 获取硬件版本号
	virtual char* getPIN();   // 获取PIN码
	virtual char* getMAC();   // 获取MAC地址
	virtual char* getWorkingMode(); // 获取工作模式
	virtual char* getModemStatus(); // 获取modem识别状态
	virtual char* getDevid();    // 获取devid

	virtual int reset(){return 1;}  // 恢复出厂设置,需要根据不同机型调用具体的子类方法

	virtual int initialInfoItem() {return 1;} // 通过老方式请求设备信息并解析返回的数据
	virtual char* getSoftwareVerOld() {return "N/A";} // 兼容老软件的获取软件版本号的旧方法
	virtual char* getHardwareVerOld() {return "N/A";} // 兼容老软件的获取硬件版本号的旧方法
	virtual char* getMACOld() {return "N/A";} // 兼容老软件的获取MAC地址的旧方法
	virtual char* getIMEIOld(){return "N/A";} // 兼容老软件的获取IMEI的旧方法
	virtual char* getMEIDOld(){return "N/A";} // 兼容老软件的获取MEID的旧方法

	// WiFi频段信息，主要针对LTE MiFi双频段设备
	virtual enum WiFiBand getWiFiBand(){return this->band;}
	virtual void setWiFiBand(enum WiFiBand band){}

	virtual int SetBandTo40(int channel, int bandwidth){return 1;}

	// IPERF test interface
	virtual double startIperfPC(const char *test_in_pc){return 0.0;}
	virtual int startIperfDUT(const char *test_in_dut){return 0;}

    // mac_suffix
    virtual int getMacSuffix() {return this->mac_suffix;}

};



/*
  7系列产品
 */
class Device_7s: public Device
{
public:
	virtual int reset();
	virtual int initialInfoItem();
	virtual char* getSoftwareVerOld();
	virtual char* getHardwareVerOld();
	virtual char* getMACOld();
	virtual char* getIMEIOld();
};



/*
  8系列产品
 */
class Device_8s: public Device
{
public:
	virtual int reset();
	virtual int initialInfoItem();
	virtual char* getSoftwareVerOld();
	virtual char* getHardwareVerOld();
	virtual char* getMACOld();
	virtual char* getIMEIOld();
};

/*
 LTE MiFi TR961 CMCC
 */
class Device_lte_cmcc: public Device_8s
{
public:
	Device_lte_cmcc(){nCount = 1;}
	virtual int initialInfoItem();
	virtual char* getSoftwareVerOld();
	virtual char* getHardwareVerOld();
	virtual char* getMACOld();
	virtual char* getIMEIOld();

	virtual int getNonce();

	virtual int reset();

	// 信道固定到40M
	virtual int SetBandTo40(int channel, int bandwidth);

private:
	CString nonce;
	int nCount;
};



/*
  LTE MiFi(M7350/TR961)
 */
#define LTE_MIFI_7350_REG				"GB"
#define LTE_MIFI_7350_WEB_REG_DEFAULT	"NONE"
#define LTE_MIFI_961_REG				"CN"
#define LTE_MIFI_7350_TEST_CHANNEL_5GHZ	"36"
#define LTE_MIFI_961_TEST_CHANNEL_5GHZ	"149"
#define LTE_MIFI_TEST_CHANNEL_2GHZ		"1"

class Device_lte: public Device_8s
{
public:
	Device_lte(){this->band = BAND_INVALID;}

	virtual int reset();
	virtual enum WiFiBand getWiFiBand();
	virtual void setWiFiBand(enum WiFiBand band);
private:
	CString SendHttpPostRequest(const CString &url,
								const CString &module_name,
								const CString &data);
	CString GetHttpToken();
};


/*
  路由+3G产品
 */
class Device_RouterAnd3G: public Device
{
public:
	virtual int reset();
};



/*
  路由产品
 */
class Device_Router: public Device
{
private:
	SOCKET sock;
	struct sockaddr_in DUT_addr;
	CString strRestoreCom;
	CString strGetware;
	CString strGetpin;
	CString strSetPassword;

public:
	virtual int reset();
};

/*
 * TPWearable
 */
class DeviceTPWearable:public Device
{

public:
	DeviceTPWearable(){}; //constructor
	~DeviceTPWearable(){}; //destructor

	virtual char* getIMEI();
	virtual char* getMEID();
	virtual char* getSoftWareVersion();
	virtual char* getHardWareVersion();
	virtual char* getPIN();
	virtual char* getMAC();
	virtual char* getWorkingMode();
	virtual char* getModemStatus();
	virtual char* getDevid();

	virtual int reset();
};

/*
 * 720pCarDv
 */
class DeviceCarDv:public DeviceTPWearable
{
private:
	int HasGotInfo;

public:
	DeviceCarDv(){ HasGotInfo = 0; this->mac_suffix = 4;}; //constructor
	~DeviceCarDv(){}; //destructor
	virtual double startIperfPC(const char *test_in_pc);
	virtual int startIperfDUT(const char *test_in_dut);

	//virtual int reset();
};
#endif

