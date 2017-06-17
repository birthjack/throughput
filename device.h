/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   device.h

  Description: �����豸�ĸ����Ŀǰ������

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-09, liguoliang      create file.
*******************************************************************************/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "diagDeviceInfo.h"

#define MAX_DEVINFO_SIZE 1024 // �����ļ��в�Ʒ��Ϣ������ַ���
#define DEVINFO_ITEM 18       // �����ļ��в�Ʒ��Ϣ�Ĳ�������

#define MAX_DEVICE_NAME_LEN 64

typedef struct DEVECEMODEL
{
	char product_name[MAX_DEVICE_NAME_LEN]; // ��Ʒ����
	unsigned short close_wifi;   // �Ƿ��Զ��ر�wifi, ֻ���·����������Ч
}DevModel;

// �豸��Ϣ�ṹ��
typedef struct DEVINFO
{
	char product_sw_ver[128];  // ��Ʒ����汾��
	char product_hw_ver[128];  // ��ƷӲ���汾��
	char product_name[64];     // ��Ʒ����
	char product_mac[20];      // ��ƷMAC��ַ
	char product_sn[16];       // ��Ʒ��SN��
	char product_imei[16];     // ��Ʒ��IMEI��
	char product_meid[16];     // ��Ʒ��MEID��
	char product_pin[10];      // ��Ʒ��PIN��
	char product_work_mode[16]; // ��Ʒ�Ĺ���ģʽ
	char product_modem_status[16];// ��Ʒ��modemʶ��״̬
	char product_devid[41];    //��Ʒdevid���
}DevInfo;


enum WiFiBand {
	BAND_INVALID,
	BAND_2_GHZ = 2,
	BAND_5_GHZ = 5
};

/*
  ��ʾ��Ʒ���͵ĸ���
 */
class Device
{
protected:
	char imei[16];         // IMEI��
	char meid[16];         // MEID��
	char mac[18];          // MAC��ַ
	char sw_version[64];   // ����汾��
	char hw_version[64];   // Ӳ���汾��
	char pin[16];          //PIN��
	char working_mode[16]; // ����ģʽ
	char modem_status[16]; // modemʶ����
	char devid[41]; //devid���

	char buff[1024];      // socket�������ݻ���

	enum WiFiBand band;	// WiFi Ƶ��

    int mac_suffix;  //SSID ��ҪMAC��λ��

public:
	Device(){this->band = BAND_2_GHZ; this->mac_suffix = 6;}
	virtual ~Device(){}

	DevInfo info;
	int initial;           // ���ɶ���ʱ��ʼ���������ʧ����0,�ɹ���1

	// ���¼����麯��Ŀǰ����ƶ���������Ʒ�ߵ����в�Ʒ��ʹ��ͳһ�ķ���ʵ��
	virtual char* getIMEI(); // ��ȡIMEI��
	virtual char* getMEID(); // ��ȡMEID��
	virtual char* getSoftWareVersion(); // ��ȡ����汾��
	virtual char* getHardWareVersion(); // ��ȡӲ���汾��
	virtual char* getPIN();   // ��ȡPIN��
	virtual char* getMAC();   // ��ȡMAC��ַ
	virtual char* getWorkingMode(); // ��ȡ����ģʽ
	virtual char* getModemStatus(); // ��ȡmodemʶ��״̬
	virtual char* getDevid();    // ��ȡdevid

	virtual int reset(){return 1;}  // �ָ���������,��Ҫ���ݲ�ͬ���͵��þ�������෽��

	virtual int initialInfoItem() {return 1;} // ͨ���Ϸ�ʽ�����豸��Ϣ���������ص�����
	virtual char* getSoftwareVerOld() {return "N/A";} // ����������Ļ�ȡ����汾�ŵľɷ���
	virtual char* getHardwareVerOld() {return "N/A";} // ����������Ļ�ȡӲ���汾�ŵľɷ���
	virtual char* getMACOld() {return "N/A";} // ����������Ļ�ȡMAC��ַ�ľɷ���
	virtual char* getIMEIOld(){return "N/A";} // ����������Ļ�ȡIMEI�ľɷ���
	virtual char* getMEIDOld(){return "N/A";} // ����������Ļ�ȡMEID�ľɷ���

	// WiFiƵ����Ϣ����Ҫ���LTE MiFi˫Ƶ���豸
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
  7ϵ�в�Ʒ
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
  8ϵ�в�Ʒ
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

	// �ŵ��̶���40M
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
  ·��+3G��Ʒ
 */
class Device_RouterAnd3G: public Device
{
public:
	virtual int reset();
};



/*
  ·�ɲ�Ʒ
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

