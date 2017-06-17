/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   configure.cpp

  Description: �������������ȫ�ֱ���

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

//��������ϵ�����Ϊȫ�ֱ���������ǰ׺"CON_"
char CON_model[MAX_DEVICE_NAME_LEN];                 // ��������
int  CON_type;                      // 0 - router, 1 - AP, 2 - NIC
int  CON_language;                  // 0 - ����, 1 - Ӣ��
char CON_DUTIP[16];                 // DUT��LAN��IP
char CON_WANIP[16];                 // DUT��WAN��IP

char CON_DATANAME[32];              // ���ݿ�����
char CON_DATAIP[16];                // ���ݿ������IP
char CON_username[32];              // ���ݿ��û���
char CON_password[16];              // ���ݿ�����
char CON_DATASERVER[32];            // ���ݿ������

char CON_LISTNAME[16];              // ��¼���Խ���ı�����


// [liguoliang start]
char CON_DUTUSER[32];               // �豸Ĭ�ϵ�¼��
char CON_DUTPASSWORD[32];           // �豸Ĭ�ϵ�¼����

int CON_EDITCONFIG;                 // �Ƿ���Ը������ò���
int CON_COMPATIBLE_OLD_SW;          // �Ƿ�����ϰ汾���(��ȡ��Ϣ��ʽ��һ��)
int CON_SET_NEW_PASSWORD;           // �Ƿ��޸Ĺ���ҳ���½����

DevModel allDev[MAX_DEV_NUM];       // �������ļ��ж�ȡ�Ļ�����Ϣ
DevInfo productWebInfo;             // ��ǰ���Բ�Ʒ��Web���������Ϣ
DevInfo productDBInfo;              // ��ǰ���Բ�Ʒ�����ݿ��������Ϣ
ConfigThroughput product_test;      // �ڽ�������������ǰ���е����ò���
ConfigInfoWirte prodcut_info_write; // �û��Ͷ�Ӧ��д�빤������

int CON_CurSel;                     // Ĭ��ѡ����豸
int CON_DevType;                    // ��Ʒ������
int CON_DBConnect;                  // �Ƿ��������ݿ�
int CON_CHECKVER;                   // �Ƿ���汾��Ϣ��MAC��ַ
int CON_FormatDisk;                 // �Ƿ��ʽ���û��̣�1-yes��0-no
int CON_3GModem;                    // �Ƿ����3G Modem�����жϣ�1-yes��0-no
int CON_WANTEST;                    // �Ƿ����WAN�ڲ���
int CON_THROUGH_TEST;               // �Ƿ�������ܲ���
int CON_AUDIO_TEST;                 // �Ƿ������Ƶ����

int CON_RESET_TIME;                 // �����λ�ȴ�ʱ��

unsigned int dev_model_id;          // �����ݿ��в�ѯ���ĵ�ǰ�豸���͵�id
unsigned long product_id;           // �����ݿ��в�ѯ���ĵ�ǰ��Ʒ��id

int WIFI_CONTROL=0;                 // �Ƿ���ʾ������WiFi����ť
int CON_DUAL_BAND_CONTROL=0;        // �Ƿ����˫Ƶ
int product_total_num = 0;          // �ܵû�������
int DUT_CONNECT = 0;                // ��ʾ�豸����״̬
HANDLE connDUTThread;               // ����豸�߳̾��
HANDLE wifiThread;                  // ���豸wifi�߳̾��
// [liguoliang end]
HANDLE connMICThread;               // ���MIC�������߳̾��
HANDLE hMutex_mic;                      // mic�߳���
HANDLE hMutex_iperf;					// iperf �߳���
HANDLE iperfDutThread;              // iperf dut ���߳�
HANDLE iperfPcThread;               // iperf pc ���߳�
HANDLE audioResultThread;           // ��Ƶ���Թ��߻�ȡ����߳�
HANDLE myTimeOutThread;             // ��ʱ�߳�

// [renjian start]
int CON_HT40 = 0;                   // ����40M�����־λ
// [renjian end]

char CON_testFilename[MAX_PATH];    // �����������ļ���
char CON_remoteIP[16];              // �Զ�IP��ַ
int  CON_passAvg_24g;                   // �������ϸ�ֵ2.4G
int  CON_passAvg_5g;                 // �������ϸ�ֵ5G
char CON_SoftVer[50];               // ��������ݿ��л�ȡ������汾
char CON_HardVer[50];               // ��������ݿ��л�ȡ��Ӳ���汾

int CON_PINcheck;                   // check PIN  1:yes,0:no
int CON_Reset;                      // reset DUT  1:yes,0:no

char sn[15];                        // SN��
char StrResult[20];                 // ���Խ���
int  pairCount;                     // pair��Ŀ
int DUTState;                       // DUT״̬
double avgThroughput[2];            // �������β��Ե�ƽ��������ֵ
char  nb[10];                        // ����Ա����
char  xb[15];	                    // �߱�
char station_number[15];            // ��λ

int passtimes=0;                    // �ϸ����
int failtimes=0;                    // ���ϸ����
double passpro=0.0;                 // һ�β���ֱͨ��
CString pt,ft,pp;                   // �ֱ𱣴���������ֵ���ַ���

char result[10000];                 // �����õ�����
CString jx;                         // ��������
char ap[100]={0};                   // ������Ϣ
CString DUTMAC;                     // ͨ��PING���DUT��MAC��ַ

HANDLE hCom;                        // ׼���򿪵Ĵ��ڵľ��
int error_no=0;                     // ���Թ����еĴ�����
int Flag=0;                         // ����Ƿ��Ѿ����ڸ�MAC����Ӧ���豸����Ϣ�ڲ��Լ�¼��

CString DUTMACSHORT;                // ��MAC��ʾ�е�-ȥ��

int CON_FORP = 2;                   // �Ƿ�ϸ� 1�ϸ�0���ϸ�2����
int CON_AUDIO_RESULT = 2;           // ��Ƶ���Խ�� 1�ϸ� 0���ϸ� 2����
CString testRecord[recordCount];    // ���Լ�¼
int counts = 0;                     // �Ѿ�����Ĳ��Լ�¼��
int begRecordPos = 0;               // ѭ�������п�ʼ���Ǹ��±�

int CON_TEST_TYPE = 0;
int CON_TEST_TIME = 20;
char CON_iperf_tx_pc[64];
char CON_iperf_tx_dut[64];
char CON_iperf_rx_pc[64];
char CON_iperf_rx_dut[64];

// MIC��������
char CON_LINE[11];
char CON_EQUIP[21];
char CON_CHECK_STATION[16];
char CON_MIC_SERVER[32];
int CON_MES_ENABLE;
int CON_ROUTE_CHECK_ENABLE;
int CON_ROUTE_CHECK_RESULT = FALSE;
int MIC_CONNECT = FALSE;
int DEVID_GET = FALSE;

// �汾��Ϣ���(���ڷ�MiFi��Ŀ)
char CON_CHECK_HARDWARE[32];
char CON_CHECK_SOFTWARE[32];


// ͨ��icmp.dll��õĺ���
pfnHV pIcmpCreateFile;
pfnBH pIcmpCloseHandle;
pfnDHDPWPipPDD pIcmpSendEcho;

// ͨ��fmifs.dll��õĺ���
PFORMATEX FormatEx;

HMODULE fmisfDll = NULL;            // ָ��fmisf.dll
HMODULE icmpDll = NULL;             // ָ��icmp.dll
HMODULE commonlibDll = NULL;
HMODULE DevIDGenDll = NULL;
HMODULE WSDataTransDll = NULL;

extern CHR_TEST_HANDLE test;
extern void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING where);


/*******************************************************************************
  Function:    initialGlobalConfigure
  Description: ��ʼ�����������ȫ�ֱ���
  Author:      liguoliang
  Date:        2013-12-06
*******************************************************************************/
void initialGlobalConfigure()
{
	product_total_num = GetPrivateProfileInt("������Ϣ", "��������", 0, JIXING);
	CON_CurSel = GetPrivateProfileInt("����", "Ĭ��ѡ��", 0, JIXING);
	CON_DevType = GetPrivateProfileInt("����", "��������", 0, JIXING);

	CON_COMPATIBLE_OLD_SW = GetPrivateProfileInt("����", "���������", 0, CONFIGURE_FILENAME);
	CON_SET_NEW_PASSWORD = GetPrivateProfileInt("����", "�޸ĵ�¼����", 1, CONFIGURE_FILENAME);
	CON_EDITCONFIG = GetPrivateProfileInt("����", "�Ƿ������", 1, CONFIGURE_FILENAME);
	WIFI_CONTROL = GetPrivateProfileInt("����", "����WIFI", 0, CONFIGURE_FILENAME);
	CON_DUAL_BAND_CONTROL = GetPrivateProfileInt("����", "�Ƿ����˫Ƶ", 0, CONFIGURE_FILENAME);

	// [liguoliang start]
	/*strcpy(CON_DUTIP, products[CON_CurSel].lan_ip);*/
	GetPrivateProfileString("DUT����", "Ĭ��IP", "192.168.1.1", CON_DUTIP, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT����", "Ĭ��user", "admin", CON_DUTUSER, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT����", "Ĭ��password", "admin", CON_DUTPASSWORD, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("DUT����", "WANIP", "131.55.66.134", CON_WANIP, 16, CONFIGURE_FILENAME);
	CON_DBConnect = GetPrivateProfileInt("����", "�������ݿ�", 1, CONFIGURE_FILENAME);
	// [liguoliang end]

	GetPrivateProfileString("���ݿ�", "���ݿ�IP", "192.168.2.1", CON_DATAIP, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("���ݿ�", "�û���", "admin", CON_username, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("���ݿ�", "����", "admin", CON_password, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("���ݿ�", "���ݿ�����", "test", CON_DATANAME, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("���ݿ�", "���ݿ������", "���߲���", CON_DATASERVER, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("���ݿ�", "������", "nx3fc", CON_LISTNAME, 16, CONFIGURE_FILENAME);

	GetPrivateProfileString("��������������", "�����ļ���", "LastTestFile.tst", CON_testFilename, MAX_PATH, CONFIGURE_FILENAME);
	GetPrivateProfileString("��������������", "�Զ�IP", "192.168.1.2", CON_remoteIP, 16, CONFIGURE_FILENAME);
	CON_passAvg_24g = GetPrivateProfileInt("��������������", "������_24g", 10, CONFIGURE_FILENAME);
	CON_passAvg_5g = GetPrivateProfileInt("��������������", "������_5g", 10, CONFIGURE_FILENAME);
	CON_RESET_TIME = GetPrivateProfileInt("��������������", "��λ�ȴ�ʱ��", 3, CONFIGURE_FILENAME)*1000;
	CON_TEST_TYPE = GetPrivateProfileInt("��������������", "��������ѡ��", 0, CONFIGURE_FILENAME);
	CON_TEST_TIME = GetPrivateProfileInt("��������������", "����ʱ��", 20, CONFIGURE_FILENAME);
	GetPrivateProfileString("��������������", "IPERF_TX_PC", "iperf -v", CON_iperf_tx_pc, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("��������������", "IPERF_TX_DUT", "iperf -v", CON_iperf_tx_dut, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("��������������", "IPERF_RX_PC", "iperf -v", CON_iperf_rx_pc, 64, CONFIGURE_FILENAME);
	GetPrivateProfileString("��������������", "IPERF_RX_DUT", "iperf -v", CON_iperf_rx_dut, 64, CONFIGURE_FILENAME);

	passtimes = GetPrivateProfileInt("����ͳ��", "���Ժϸ����", 10, CONFIGURE_FILENAME);
	failtimes = GetPrivateProfileInt("����ͳ��", "���Բ��ϸ����", 10, CONFIGURE_FILENAME);

	char passpro_string[10];
	GetPrivateProfileString("����ͳ��","һ�β���ֱvͨ��", "0.0", passpro_string, 10, CONFIGURE_FILENAME);
	passpro = atof(passpro_string);

	// [renjian start] MIC
	GetPrivateProfileString("MIC", "�����߱��", "4FD-2", CON_LINE, 11, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "�����豸����", "1", CON_EQUIP, 21, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "��һ����վ����", "T10", CON_CHECK_STATION, 16, CONFIGURE_FILENAME);
	GetPrivateProfileString("MIC", "MIC��������ַ", "mictest.rd.tp-link.net", CON_MIC_SERVER, 32, CONFIGURE_FILENAME);

	CON_MES_ENABLE = GetPrivateProfileInt("MIC", "MESʹ��", 0, CONFIGURE_FILENAME);
	CON_ROUTE_CHECK_ENABLE = GetPrivateProfileInt("MIC", "���̼��ʹ��", 0, CONFIGURE_FILENAME);
	// [renjian end]

	// �汾��Ϣ����(���ڷ�MIFI��Ŀ)
	GetPrivateProfileString("�汾���", "Ӳ���汾", "N/A", CON_CHECK_HARDWARE, 32, CONFIGURE_FILENAME);
	GetPrivateProfileString("�汾���", "����汾", "N/A", CON_CHECK_SOFTWARE, 32, CONFIGURE_FILENAME);
}

/*******************************************************************************
  Function:    LoadDLLFunctions
  Description: �������dll��̬���ӿ⣬�������еĺ���������Ӧ�Ĳ���
  Author:      liguoliang
  Date:        2013-12-03
*******************************************************************************/
BOOLEAN LoadDLLFunctions()
{
	fmisfDll = LoadLibrary( "fmifs.dll" ); // ����fmifs.dll��,���и�ʽ������
	icmpDll = LoadLibrary("icmp.dll"); // ����icmp.dll��,����ping����

	if (fmisfDll == NULL || icmpDll == NULL) {
		return FALSE;
	}

	// �ҵ�ִ�и�ʽ��������FormatEx������ַ
	FormatEx = (PFORMATEX) GetProcAddress( fmisfDll,"FormatEx" );

	// �ҵ�ִ�з���arp������غ�����ַ
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
  Description: ��ʼ��hCom���ھ��
  Author:      liguoliang
  Date:        2013-12-09
*******************************************************************************/
BOOLEAN initialCOM()
{
	//��ʼ�����ڹ���
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

	//��ʼ��dcb���󶨵�hCom��
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

	COMMTIMEOUTS timeOver ; //���峬ʱ�ṹ������д�ýṹ
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
  Description: ���ַ����ж�ȡ����е�ֵ
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

