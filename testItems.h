/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   testItems.h

  Description: ����������ʵ�ֵĺ�������ԭ����ͬһ���ļ��ж���ĺ����������

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

// �����ڵ��û�ȡ�豸��Ϣ��ʱ�򷵻صĴ������
typedef enum{
	MATCH_VER = 1,
	DISCONNECT_DUT = -1,
	UNKNOWN_PASSWORD = -2,
	UNMATCH_SW_VER = -3,
	UNMATCH_HW_VER = -4,
	UNMATCH_MAC = -5,
	UNMATCH_PIN = -6
}GET_VER_ERROR;

#define  TEST_NUM 1  // ѭ�����ԵĴ���


// �������ͨ���Ĵ������
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
*�� �� ����   throughputTest
*ժ    Ҫ��   ����������������һ�β��ϸ����ٲ�һ�Σ�
*             �ڸú����У���Ҫ�ǵ���beginThroughputTest()
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-15
*********************************************************/
DWORD WINAPI throughputTest(LPVOID pParam);

/*******************************************************************************
  Function:    openWiFiThread
  Description: һֱ����WiFi֪��ֹͣΪֹ
  Author:      liguoliang
  Date:        2013-12-25
*******************************************************************************/
DWORD WINAPI openWiFiThread(LPVOID pParam);

bool pingWAN(char IPaddr[16]);


/*******************************************************************************
  Function:    getMACInfo
  Description: ͨ������ARP����ȡMAC��ַ
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
int getMACInfo(char *ip, char *resultBuf);

/*********************************************************
*�� �� ����   insertTestRecord
*ժ    Ҫ��   �����Լ�¼��ӵ���recordCount��Ԫ�ص�ѭ��������
*��    �ߣ�   �˹�ΰ
*�������ڣ�   2007-10-22
*********************************************************/
void insertTestRecord(double testValue, int throughput_threshold);

/*******************************************************************************
  Function:    formatDisk
  Description: ���7Щ���д洢�̵ĵĻ��ͣ���ʽ���洢��
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int formatDisk();

/*******************************************************************************
  Function:    AddError
  Description: ����CMyDlg�е�AddError�����ɵ����ĺ���
  Author:      liguoliang
  Date:        2013-12-04
*******************************************************************************/
void AddError(int er);

/*******************************************************************************
  Function:    connectDev
  Description: ����DUT�ͶԷ�����
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN connectDev();

/*******************************************************************************
  Function:    saveFileLog
  Description: �������������Խ����log�ļ���
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN saveFileLog(CString avg, CTime time);

/*******************************************************************************
  Function:    testWAN
  Description: ����WAN��
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testWAN();

/*******************************************************************************
  Function:    testVersion
  Description: �����Ӳ���汾����ҳ��ʾ��MAC��ַ�Ƿ�ƥ��ȹ���
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int testVersion();

/*******************************************************************************
  Function:    resetDUT
  Description: �����λ
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
int resetDUT();

/*******************************************************************************
  Function:    testPIN
  Description: ���PIN��
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int testPIN();

/*******************************************************************************
  Function:    throughputPass
  Description: ��¼�������ϸ�
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputPass(CString throughput_value);

/*******************************************************************************
  Function:    throughputNoPass
  Description: ��¼���������ϸ�
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int throughputNoPass(CString throughput_value);

/*******************************************************************************
  Function:    formatDUT
  Description: ��ʽ���û���
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int formatDUT();

/*******************************************************************************
  Function:    test3GModem
  Description: ���3G Modem�Ƿ�����
  Author:      liguoliang
  Date:        2013-12-10
*******************************************************************************/
int test3GModem();

/*******************************************************************************
  Function:    testMIC
  Description: ���MIC״̬
  Author:      renjian
  Date:        2015-10-29
*******************************************************************************/
int testMIC();

/*******************************************************************************
  Function:    testType
  Description: �������ѡ״̬
  Author:      renjian
  Date:        2016-10-31
*******************************************************************************/
int testType();

/*******************************************************************************
  Function:    getDBProductInfo
  Description: �����ݿ��м��MAC��ַ��������Ϣд�����
  Author:      liguoliang
  Date:        2013-12-18
*******************************************************************************/
int getDBProductInfo();

/*******************************************************************************
  Function:    checkExistDBLog
  Description: ����MAC��ѯ�Ƿ��Ѿ����ڶ�Ӧ�Ĳ�Ʒ�Ĳ��Լ�¼
  Author:      liguoliang
  Date:        2013-12-19
*******************************************************************************/
int checkExistDBLog();

/*******************************************************************************
  Function:    changeDBProductStatus
  Description: �޸����ݿ���Product��Ĵ�����Խ����p_status�ֶε�ֵ
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBProductStatus(int success_or_failed);

/*******************************************************************************
  Function:    deleteDBPassLog
  Description: �޸�ԭ�����Գɹ��ļ�¼������Product���status�ֶ�
  Author:      liguoliang
  Date:        2013-12-23
*******************************************************************************/
int changeDBPassLog(ULONG tl_id);

/*******************************************************************************
  Function:    getDevInfo
  Description: ��ȡ�豸��Ϣ����ʾ�ڽ�����
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
int getDevInfo();

/*******************************************************************************
  Function:    iperfTest
  Description: iperf���߲��Խӿ�
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void iperfTest();

/*******************************************************************************
  Function:    chariotTest
  Description: chariot���߲��Խӿ�
  Author:      renjian
  Date:        2016-04-16
*******************************************************************************/
void chariotTest();

/*******************************************************************************
  Function:    autioTest
  Description: ��Ƶ���Թ��߽ӿ�
  Author:      renjian
  Date:        2016-05-24
*******************************************************************************/
void audioTest();

/*******************************************************************************
  Function:    audioListen
  Description: ��Ƶ���Žӿ�
  Author:      renjian
  Date:        2016-05-31
*******************************************************************************/
void audioListen();

/*******************************************************************************
  Function:    fixBandTo40
  Description: ���ù̶��ŵ��������ŵ���������Ϊ40M
  Author:      renjian
  Date:        2015-01-07
*******************************************************************************/
int fixBandTo40(int channel, int bandwidth);

/*******************************************************************************
  Function:    connectDUTThread
  Description: �����豸�̣߳����̻߳���ѡ����ͺ�һֱ���м���豸
  Author:      liguoliang
  Date:        2013-12-27
*******************************************************************************/
DWORD WINAPI connectDUTThread(LPVOID pParam);

/*******************************************************************************
  Function:    connectMICThread
  Description: ����MIC�������̣߳����̻߳���ѡ����ͺ�һֱ���м��MIC������
  Author:      renjian
  Date:        2015-10-20
*******************************************************************************/
DWORD WINAPI connectMICThread(LPVOID pParam);

/*******************************************************************************
  Function:    mac_compare
  Description: �Լ������mac�ȽϷ���
  Author:      liguoliang
  Date:        2014-09-05
*******************************************************************************/
int mac_compare(char* mac1, char* mac2);

/*******************************************************************************
  Function:    setField
  Description: ��ҵվ�������ݸ�ֵ
  Author:      renjian
  Date:        2015-10-09
*******************************************************************************/
int setField(char buffer[], int bufsiz, const char * valstr);

/*******************************************************************************
  Function:    uploadData
  Description: �����ϴ���MIC������
  Author:      renjian
  Date:        2015-10-09
*******************************************************************************/
int uploadData(CString avgThroughput, char starttime[], char endtime[], int throughput_threshold);

#endif