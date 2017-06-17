/* 
 * Description:	WSDataTransCpp.h v1.11.0 Build20160517
 * Create:		RuiJiankun, 2014-xx-xx
 * Modify:	
 *		2014-9-10	chengengsheng	Add Version Description
 *		2014-9-18	chengengsheng	Modify Comments; Add DATABase field length definitions and change the structures
 */

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WSDATATRANSCPP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WSDATATRANSCPP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WSDATATRANSCPP_EXPORTS
#define WSDATATRANSCPP_API __declspec(dllexport)
#else
#define WSDATATRANSCPP_API __declspec(dllimport)
#endif

// +++++++++++++++++++++++++++++++++++++++ //
// ++++ MIC���ݿ��ж�����ύ�ֶγ��� ++++ //
// +++++++++++++++++++++++++++++++++++++++ //
#define FIELD_LEN_DEVICEID			50
#define FIELD_LEN_HARDWAREID		50
#define FIELD_LEN_MACID				12
#define FIELD_LEN_SERIALNO			50
#define FIELD_LEN_RESULT_PF			1
#define FIELD_LEN_MONO				50
#define FIELD_LEN_PRODUCTNO			50
#define FIELD_LEN_PRODUCTVERSION	3

#define FIELD_LEN_TRANSACTIONID		16
#define FIELD_LEN_TIME				64
#define FIELD_LEN_OPNO				50
#define FIELD_LEN_FAILCODE			50
#define FIELD_LEN_LINE				10
#define FIELD_LEN_WORKSTATION		10
#define FIELD_LEN_USERNO			10
#define FIELD_LEN_EQUIPMENT_NO		20
#define FIELD_LEN_EQUIPMENT_VER		20
#define FIELD_LEN_TOOL_VER			50
#define FIELD_LEN_FAILMSG			100
#define FIELD_LEN_LOGFILENAME		100

#define FIELD_LEN_RESULT_VALUE		10
#define FIELD_LEN_STRING_VALUE		50
#define FIELD_LEN_RESERVED			4000
#define FIELD_LEN_RESERVED_KEY		64
#define FIELD_LEN_RESERVED_VALUE	512

#define FIELD_LEN_T10_MODE			20
#define FIELD_LEN_T10_PWRGAIN		100
#define FIELD_LEN_T10_TEMPCAL		16
#define FIELD_LEN_T10_FREQCAL		16
#define FIELD_LEN_T10_FIRMWAREID	50
#define FIELD_LEN_T10_HWIDDESCRIP	512

#define FIELD_LEN_T20_TESTID		10
#define FIELD_LEN_T20_PORT			10
#define FIELD_LEN_T20_VCT			4

#define FIELD_LEN_T30_MACIDQTY		10
#define FIELD_LEN_T30_MACSTATE		4
#define FIELD_LEN_T30_PINVALUE		8
#define FIELD_LEN_T30_PINSTATE		4
#define FIELD_LEN_T30_SIGNSTATE		4
#define FIELD_LEN_T30_PIBVERSION	20

#define FIELD_LEN_T40_STRING		50
#define FIELD_LEN_T40_CFGCODE		20
#define FIELD_LEN_T40_MODE			20

// ++++++++++++++++ //
// ++++ MACROs ++++ //
// ++++++++++++++++ //
#define RESERVED_FLASHUID_KEY		"FLASHUID"

/// <summary>
/// ���Խ���Ĵ��룬����ÿ��Identity�е�Failcode������
/// </summary>
typedef enum _FAIL_CODE
{
	T10_PASS		=	10000,
	T10_FAIL_CAL	=	10001,
	T10_FAIL_PWR	=	10002,
	T10_FAIL_PPM	=	10003,
	T10_FAIL_EVM	=	10004,
	T10_FAIL_SENS	=	10005,
	T10_FAIL_RESV	=	10006,
	
	T20_PASS		=	20000,	
	T20_FAIL_VCT	=	20001,
	T20_FAIL_SWITCH	=	20002,
	T20_FAIL_L2W	=	20003,
	T20_FAIL_RESV	=	20004,
	
	T30_PASS		=	30000,
	T30_FAIL_MAC	=	30001,
	T30_FAIL_PIN	=	30002,
	T30_FAIL_SIGN	=	30003,
	T30_FAIL_ROUTING_CHK_FAIL		=	30004,
	T30_FAIL_ROUTING_CHK_NONE		=	30005,
	T30_FAIL_DUP_WRITTEN			=	30006,
	T30_FAIL_SYS					=	30007,
	T30_FAIL_ROUTING_CHK_INTERRUPT	=	30008,
	T30_FAIL_WRITTEN_INTERRUPT		=	30009,
	T30_FAIL_RESET_INTERRUPT		=	30010,
	T30_FAIL_RESV					=	30011,
	
	T40_PASS		=	40000,
	T40_FAIL_PERF	=	40001,
	T40_FAIL_CONN	=	40002,
	T40_FAIL_RESV	=	40003,

	T50_PASS		=	50000,
    T50_FAIL_RESV,

    T60_PASS		=	60000,
    T60_FAIL_RESV,

    T70_PASS		=	70000,
    T70_FAIL_RESV,

    T80_PASS		=	80000,
    T80_FAIL_RESV,

    T90_PASS		=	90000,
    T90_FAIL_RESV,
    T91_PASS		=	91000,
    T91_FAIL_RESV,
    T92_PASS		=	92000,
    T92_FAIL_RESV,
    T93_PASS		=	93000,
    T93_FAIL_RESV,
    T94_PASS		=	94000,
    T94_FAIL_RESV,
    T95_PASS		=	95000,
    T95_FAIL_RESV,
    T96_PASS		=	96000,
    T96_FAIL_RESV,
    T97_PASS		=	97000,
    T97_FAIL_RESV,
    T98_PASS		=	98000,
    T98_FAIL_RESV,

    T99_PASS		=	99000,
    T99_FAIL_RESV,
}FAIL_CODE;


/// <summary>
/// ����ÿһ����intΪ����ֵ���͵Ľӿ�ʱ�������жϷ���ֵ��ö�١�
/// </summary>
typedef enum _DATATRANS_RESULT
{
	ERR_MAC_NOT_IN_SYSTEM       =   -16,//MAC�Ǳ�ϵͳ����
    ERR_MAC_BINDED_OTHER_DEV    =   -15,//MAC�Ѱ󶨹������豸
    ERR_GET_DEVID_BY_MAC        =   -14,//GetDeviceidByMac����ʧ��
    ERR_ICE_INIT                =   -13,//ICE�����ʼ��ʧ��           
    ERR_ICE_DEFAULT             =   -12,//ICE���񽻻�ʧ��  
	ERR_DEVICEID_DUP_FOUND      =   -11,//���ֵ�ǰ�豸ʹ�����ظ���DeviceID
    ERR_DEVICEID_DUP_CHECK      =   -10,//�ظ�DeviceID���ؼ��ʧ��
	ERR_MAC_BINDING_FORBIDDEN   =   -9, //��ǰ��ҵվ���ݲ������MAC�ϴ�
	ERR_PARA_IS_NULL            =   -8, //DataUpload���ϴ���Parameter���ݽṹδ��ֵ
    ERR_IDN_IS_NULL             =   -7, //DataUpload���ϴ���Identity���ݽṹδ��ֵ
	ERR_DEVID_ILLEGAL           =   -6, //DataUpload���ϴ���DeviceID���Ϸ������Ȳ�Ϊ40/���ڷ�16�����ַ�/ȫ0/ȫF��
    ERR_MAC_ILLEGAL             =   -5, //DataUpload���ϴ���MAC���Ϸ������Ȳ�Ϊ12/���ڷ�16�����ַ�/ȫ0��
	ERR_MAC_BINDED_OTHER_SN     =   -4, //MAC��SN��ʱ��֮ǰ��SN�Ѻ�����MAC�󶨹�
    ERR_DATA_LENGTH             =   -3, //����XML�ļ���ĳ�����ݵĳ��ȳ��������ݿⶨ��ĳ���
    ERR_XML_FORMAT              =   -2, //�����XML��ʽ����
    ERR_SYS                     =   -1, //���ýӿ�ʱ����ϵͳ���󣬽�����ִ�����ʱ����һ��ConnectCheck()
    OK                          =   0,  //һ��OK
    ROUTING_CHECK_FAIL          =   1,  //���̼�飺��ѯ��DUT��ָ��վ����Բ�ͨ��
    ROUTING_CHECK_NO_TEST       =   2,  //���̼�飺��ѯ��DUT��ָ��վ��δ����
    DATAUP_EXIST_DEVID          =   3,  //DeviceID��MAC��ʱ��֮ǰ��DeviceID�Ѿ�������MAC��
    DATAUP_EXIST_MAC            =   4,  //DeviceID��MAC��ʱ��֮ǰ��MAC��ַ��������DevID��
    DATAUP_EXIST_MAC_DIF_SN     =   5,  //MAC��SN��ʱ��֮ǰ��MAC��ַ�Ѻ�����SN�󶨹�
}DATATRANS_RESULT;

/// <summary>
/// ����ObtainDeviceIDwithTypeʱ��Ҫ�����MIC��Ʒ����в�Ʒ�����塣
/// </summary>
typedef enum _PRODUCT_TYPE
{
	// ����MIC��Ʒ���
    NX_WIRELESSROUTER       =   0,
    NX_IPCAMERA             =   1,
    NX_SMARTROUTER          =   2,
    NX_REMOTECONTROL        =   3,
    NX_SMARTPLUG            =   4,
    NX_DOORSENSOR           =   5,
    NX_DOORBELL             =   6,
    NX_3G4GROUTER           =   7,
    NX_WIRELESSADROUTER     =   8,
    NX_SMBROUTER            =   9,
    NX_SMBWIRELESSROUTER    =   10,
    NX_WIFISPEAKER          =   11,
    NX_CEILINGMOUNTAP       =   12,
    NX_WALLJACKAP           =   13,
    NX_OUTDOORAP            =   14,
    NX_CPEBS                =   15,
	NX_AC					=	16,
    NX_DESKTOPAP			=	17,
    NX_HYFIEXTENDER			=	18,
    NX_L2MANAGEDSWITCH		=	19,
    NX_L3MANAGEDSWITCH		=	20,
    NX_REPEATER				=	21,
    NX_SMARTSWITCH			=	22,
    NX_NETWORKVIDEORECORDER	=	23,
    NX_SURVEILLANCECAMERA	=	24,
	NX_WEARABLECAMERA		=	25,
	NX_CARDV				=	26,
	// �����з���������
	NX_SAMPLE				=	998,
	// ����MIC��Ʒ���
    WX_WIRELESSROUTER       =   1000,
    WX_BROADBANDACCESS      =   1001,
    WX_RANGEEXTENDER        =   1002,
    WX_POWERLINE            =   1003,
    WX_IOT                  =   1004,
    WX_IPCAMERA             =   1005,
    WX_IOT_SMARTPLUGSWITCH  =   1006,
	WX_XDSLMODEMROUTER		=	1007,
    WX_LTEGATEWAY			=	1008,
    WX_CABLEMODEMROUTER		=	1009,
    WX_PON					=	1010,
    WX_MIFI					=	1011,
    WX_SMBROUTER			=	1012,
    WX_SMBSWITCH			=	1013,
    WX_CEILINGMOUNTAP		=	1014,
    WX_OUTDOORAP			=	1015,
    WX_DESKTOPAP			=	1016,
	WX_IOT_SENSOR			=	1017,
    WX_IOT_SMARTBULB		=	1018,
	WX_IOT_RESMARTPLUG		=	1019,
	WX_EAP_WALLPLATE		=	1020,
	WX_EAP_OUTDOOR			=	1021,
	WX_IOT_ROUTER			=	1022,
	// �����з���������
	WX_SAMPLE				=	1998,
}PRODUCT_TYPE;

//----------T10----------
class WSDATATRANSCPP_API CT10Identity {
public:
	CT10Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
	char Hwiddescrip[FIELD_LEN_T10_HWIDDESCRIP+1];	// ���128byte���ַ���
	char Firmwareid[FIELD_LEN_T10_FIRMWAREID+1];// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];

	char Guid[FIELD_LEN_STRING_VALUE+1];
    char Pid[FIELD_LEN_STRING_VALUE+1];
    char Vid[FIELD_LEN_STRING_VALUE+1];
    char Nationcode[FIELD_LEN_STRING_VALUE+1];
};

class WSDATATRANSCPP_API CT10Parameter {
public:
	CT10Parameter();

	char Chain[FIELD_LEN_RESULT_VALUE+1];
	char Mode[FIELD_LEN_T10_MODE+1];
	char Channel[FIELD_LEN_RESULT_VALUE+1];
	char Rate[FIELD_LEN_RESULT_VALUE+1];

	char Power[FIELD_LEN_RESULT_VALUE+1];
	char Ppm[FIELD_LEN_RESULT_VALUE+1];
	char Evm[FIELD_LEN_RESULT_VALUE+1];
	char Evmpwr[FIELD_LEN_RESULT_VALUE+1];
	char Sens[FIELD_LEN_RESULT_VALUE+1];
	char Rssi[FIELD_LEN_RESULT_VALUE+1];
	char Loss[FIELD_LEN_RESULT_VALUE+1];
	char Pwrgain[FIELD_LEN_T10_PWRGAIN+1];
	char Tempcal[FIELD_LEN_T10_TEMPCAL+1];
	char Freqcal[FIELD_LEN_T10_FREQCAL+1];
	char Prepwr[FIELD_LEN_RESULT_VALUE+1];
    char Fcalpwr[FIELD_LEN_RESULT_VALUE+1];
    char Gaincount[FIELD_LEN_RESULT_VALUE+1];

	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	/// <summary>
    /// ����һ��ؼ��ֺͶ�Ӧֵ��Ԥ���ֶ���
    /// </summary>
    /// <param name="key">�ؼ��֣����������ֶε�����</param>
    /// <param name="value">ֵ�����������ֶε�ֵ</param>
    /// <returns>����true����ӳɹ�������ʧ��</returns>
	bool AddReserved(const char * key, const char * value);
};

//----------T20----------
class WSDATATRANSCPP_API CT20Identity {
public:
	CT20Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];

	char Equipmentver[FIELD_LEN_EQUIPMENT_VER+1];
	char Testid[FIELD_LEN_T20_TESTID+1];
};

class WSDATATRANSCPP_API CT20Parameter {
public:
	CT20Parameter();
	char Port[FIELD_LEN_T20_PORT+1];

    char Vct[FIELD_LEN_T20_VCT+1];
    char Spd_10MH[FIELD_LEN_RESULT_VALUE+1];
    char Crc_10MH[FIELD_LEN_RESULT_VALUE+1];
    char Spd_10MF[FIELD_LEN_RESULT_VALUE+1];
    char Crc_10MF[FIELD_LEN_RESULT_VALUE+1];
    char Spd_100MH[FIELD_LEN_RESULT_VALUE+1];
    char Crc_100MH[FIELD_LEN_RESULT_VALUE+1];
    char Spd_100MF[FIELD_LEN_RESULT_VALUE+1];
    char Crc_100MF[FIELD_LEN_RESULT_VALUE+1];
    char Spd_1000M[FIELD_LEN_RESULT_VALUE+1];
    char Crc_1000M[FIELD_LEN_RESULT_VALUE+1];
    char Spd_10GM[FIELD_LEN_RESULT_VALUE+1];
    char Crc_10GM[FIELD_LEN_RESULT_VALUE+1];
	char L2w[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T30----------
class WSDATATRANSCPP_API CT30Identity {
public:
	CT30Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];

    char Macidqty[FIELD_LEN_T30_MACIDQTY+1];
};

class WSDATATRANSCPP_API CT30Parameter {
public:
	CT30Parameter();

	char Macstate[FIELD_LEN_T30_MACSTATE+1];
    char Pinvalue[FIELD_LEN_T30_PINVALUE+1];
    char Pinstate[FIELD_LEN_T30_PINSTATE+1];
    char Signstate[FIELD_LEN_T30_SIGNSTATE+1];
	char Pibversion[FIELD_LEN_T30_PIBVERSION+1];

	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T40----------
class WSDATATRANSCPP_API CT40Identity {
public:
	CT40Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];

    char Cfgcode[FIELD_LEN_T40_CFGCODE+1];
    char Sutname[FIELD_LEN_T40_STRING+1];
    char Sutfw[FIELD_LEN_T40_STRING+1];
    char Sutpcos[FIELD_LEN_T40_STRING+1];
    char Loss[FIELD_LEN_RESULT_VALUE+1];
    char Dutname[FIELD_LEN_T40_STRING+1];
    char Dutfw[FIELD_LEN_T40_STRING+1];
    char Dutpcos[FIELD_LEN_T40_STRING+1];
};

class WSDATATRANSCPP_API CT40Parameter {
public:
	CT40Parameter();
	char Mode[FIELD_LEN_T40_MODE+1];

	char Channel[FIELD_LEN_RESULT_VALUE+1];
	char Tcp_up[FIELD_LEN_RESULT_VALUE+1];
	char Tcp_down[FIELD_LEN_RESULT_VALUE+1];
	char Rssi[FIELD_LEN_RESULT_VALUE+1];
	char Threshold[FIELD_LEN_RESULT_VALUE+1];
	char Scriptname[FIELD_LEN_STRING_VALUE+1];
    char Chariottime[FIELD_LEN_RESULT_VALUE+1];
    char Uppairs[FIELD_LEN_RESULT_VALUE+1];
    char Downpairs[FIELD_LEN_RESULT_VALUE+1];

	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T50----------
class WSDATATRANSCPP_API CT50Identity {
public:
	CT50Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];
};

class WSDATATRANSCPP_API CT50Parameter {
public:
	CT50Parameter();

	char Uprate[FIELD_LEN_RESULT_VALUE+1];
    char Downrate[FIELD_LEN_RESULT_VALUE+1];
    char Upnoise[FIELD_LEN_RESULT_VALUE+1];
    char Downnoise[FIELD_LEN_RESULT_VALUE+1];
    char Traintime[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T60----------
class WSDATATRANSCPP_API CT60Identity {
public:
	CT60Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];
};

class WSDATATRANSCPP_API CT60Parameter {
public:
	CT60Parameter();

	char Dut1_mac[FIELD_LEN_MACID+1];
    char Dut2_mac[FIELD_LEN_MACID+1];
    char Dut1_ssid[FIELD_LEN_STRING_VALUE+1];
    char Dut2_ssid[FIELD_LEN_STRING_VALUE+1];
    char Lan1_phyrate1[FIELD_LEN_RESULT_VALUE+1];
	char Lan1_phyrate2[FIELD_LEN_RESULT_VALUE+1];
	char Lan1_throughput[FIELD_LEN_RESULT_VALUE+1];
	char Lan2_phyrate1[FIELD_LEN_RESULT_VALUE+1];
	char Lan2_phyrate2[FIELD_LEN_RESULT_VALUE+1];
	char Lan2_throughput[FIELD_LEN_RESULT_VALUE+1];
	char Lan3_phyrate1[FIELD_LEN_RESULT_VALUE+1];
	char Lan3_phyrate2[FIELD_LEN_RESULT_VALUE+1];
	char Lan3_throughput[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T70----------
class WSDATATRANSCPP_API CT70Identity {
public:
	CT70Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];
};

class WSDATATRANSCPP_API CT70Parameter {
public:
	CT70Parameter();
	char Chain[FIELD_LEN_RESULT_VALUE+1];
	char Channel[FIELD_LEN_RESULT_VALUE+1];

    char Txpower[FIELD_LEN_RESULT_VALUE+1];
	char Freqoffset[FIELD_LEN_RESULT_VALUE+1];
	char Freqdrift[FIELD_LEN_RESULT_VALUE+1];
	char Biterrorrate[FIELD_LEN_RESULT_VALUE+1];
	char Framerrorrate[FIELD_LEN_RESULT_VALUE+1];
	//char S_field[FIELD_LEN_RESULT_VALUE+1];
	//char B_field[FIELD_LEN_RESULT_VALUE+1];
	char Timeaccuracy[FIELD_LEN_RESULT_VALUE+1];
	char Positivejit[FIELD_LEN_RESULT_VALUE+1];
	char Negativejit[FIELD_LEN_RESULT_VALUE+1];
	char SFieldLower[FIELD_LEN_RESULT_VALUE+1];
	char SFieldUpper[FIELD_LEN_RESULT_VALUE+1];
	char BFieldLower[FIELD_LEN_RESULT_VALUE+1];
	char BFieldUpper[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T80----------
class WSDATATRANSCPP_API CT80Identity {
public:
	CT80Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];
};

class WSDATATRANSCPP_API CT80Parameter {
public:
	CT80Parameter();

    char Halfload[FIELD_LEN_RESULT_VALUE+1];
	char Fullload[FIELD_LEN_RESULT_VALUE+1];
	char Overload[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------T99----------
class WSDATATRANSCPP_API CT99Identity {
public:
	CT99Identity();

    char Result[FIELD_LEN_RESULT_PF+1];
    FAIL_CODE Failcode;
	char Failmsg[FIELD_LEN_FAILMSG+1];
	char Logfilename[FIELD_LEN_LOGFILENAME+1];
    char Deviceid[FIELD_LEN_DEVICEID+1];		// 40Byte��ʮ�������ַ���
    char Macid[FIELD_LEN_MACID+1];				// 12Byte��ʮ�������ַ������м�û�к��
    char Serialno[FIELD_LEN_SERIALNO+1];
	char Hardwareid[FIELD_LEN_HARDWAREID+1];	// 32Byte��ʮ�������ַ���
    char Line[FIELD_LEN_LINE+1];
    char Workstation[FIELD_LEN_WORKSTATION+1];
    char Userno[FIELD_LEN_USERNO+1];
    char Equipmentno[FIELD_LEN_EQUIPMENT_NO+1];
    char Starttime[FIELD_LEN_TIME+1];
    char Endtime[FIELD_LEN_TIME+1];
    char Ver[FIELD_LEN_TOOL_VER+1];
};

class WSDATATRANSCPP_API CT99Parameter {
public:
	CT99Parameter();

    char Resetstate[FIELD_LEN_RESULT_VALUE+1];
	char Opcount[FIELD_LEN_RESULT_VALUE+1];
	char Oporder[FIELD_LEN_RESULT_VALUE+1];
	
	// ��CT10Parameter�еĶ���
	char Reserved[FIELD_LEN_RESERVED+1];	// Readonly
	bool AddReserved(const char * key, const char * value);
};

//----------Data Transfer----------
// This class is exported from the WSDataTransCpp.dll
class WSDATATRANSCPP_API CWSDataTransCpp {
public:
	CWSDataTransCpp(char * serverIP, int wsTimeout = 3000);

	/// <summary>
    /// ����ģʽ��������Ϊtrueʱ��DataUpload������������ύ���ݣ��������ݻ��浽����
    /// </summary>
	bool IsBufMode;

    /// <summary>
    /// ��ȡDeviceID
    /// </summary>
	/// <param name="line">�����߱���</param>
	/// <param name="station">���빤��վ</param>
    /// <returns>��ȡ����DeviceID��ʧ���򷵻ؿ��ַ���</returns>
    char* ObtainDeviceID(const char* line, const char* station);
	
	/// <summary>
    /// �����ϴ�������ļ�
    /// </summary>
    /// <param name="OpNo">����վ��(����)���ַ�������</param>
	/// <param name="lenth">���������ϴ�ʧ�ܽ���ĸ���</param>
    /// <returns>�����ϴ��������ַ������,�����ش���ʧ�ܵĽ������</returns>
    char** BufferDataUpload(const char* OpNo,int *lenth);

	/// <summary>
    /// ��ȡ��ǰ�ڴ����DeviceID������
    /// </summary>
	int GetDevIDCount();

	/// <summary>
    /// ���������Ӽ�麯��
    /// </summary>
    /// <returns>true���������������ӣ�false�����������ӳ���</returns>
    bool ConnectCheck();
       
	/// <summary>
    /// ���̼�麯����deviceID��mac��sn����������һ�����ɣ�����Ϊ���ַ���
    /// </summary>
    /// <param name="currentStation">��ǰ����վ����ţ�����"T20"��"T30"</param>
    /// <param name="checkStation">Ҫ����վ����ţ�����"T10"��"T20"</param>
    /// <param name="deviceID">deviceIDΪ����40��ʮ�������ַ���</param>
    /// <param name="mac">����Ϊ12��ʮ�������ַ������м�û�к��</param>
    /// <param name="sn">ͨ������Ϊ13���ַ�������Ʒ���к�</param>
    /// <returns>���̼����</returns>
	int RoutingCheck(const char* currentStation, 
					 const char* checkStation, 
					 const char* deviceID, 
					 const char* mac, 
					 const char* sn = "");
        
    /// <summary>
    /// ��ѯָ��mac��ַ��Ӧ��sn��
    /// </summary>
    /// <param name="mac">����mac,����Ϊ12��ʮ�������ַ������м�û�к��</param>
    /// <returns>��Ӧ��sn��</returns>
    char* GetSN(const char* mac);
    
    /// <summary>
    /// ��ѯָ��sn�Ŷ�Ӧ��mac��ַ
    /// </summary>
    /// <param name="sn">����sn</param>
    /// <returns>��Ӧ��mac��ַ,����Ϊ12��ʮ�������ַ������м�û�к��</returns>
    char* GetMAC(const char* sn);

	/// <summary>
    /// ��ѯָ��mac��ַ��Ӧ��DeviceID
    /// </summary>
    /// <param name="mac">����mac,����Ϊ12��ʮ�������ַ������м�û�к��</param>
	/// <param name="lenth">���DeviceID�б���</param>
    /// <returns>��Ӧ��DeviceID�ַ����б�ָ�룬�����ж��DeviceID,��һ��Ϊ���Ȱ󶨵�DeviceID�����һ��Ϊ���󶨵�DeviceID</returns>
    char** GetDevidByMac(const char* mac, int* length);
    
    /// <summary>
    /// ��ѯָ��DeviceID��Ӧ��mac��ַ
    /// </summary>
    /// <param name="sn">����deviceid������Ϊ40��ʮ�������ַ���</param>
	/// <param name="lenth">���DeviceID�б���</param>
    /// <returns>��Ӧ��mac��ַ�б�ָ��, ��һ��Ϊ���Ȱ󶨵�MAC�����һ��Ϊ���󶨵�MAC</returns>
    char** GetMacByDevid(const char* devid, int* length);

	/// <summary>
    /// ����Ƿ�Ϊϵͳ�����MAC
    /// </summary>
    /// <param name="mac">������MAC</param>
    /// <returns>���󷵻�ֵ</returns>
	int CheckMacOfServer(const char* mac);

	/// <summary>
    /// ���MAC�ظ���������Ƿ����һ��MAC�󶨶��DeviceID�������,ͬʱ����Ƿ�Ϊϵͳ�����MAC
    /// </summary>
    /// <param name="devid">��ǰ�豸��DeviceID</param>
    /// <param name="mac">��ǰ�豸��MAC</param>
    /// <returns>���󷵻�ֵ</returns>
	int CheckBindingsByMac(const char* devid, const char* mac);

	/// <summary>
    /// ��ȡDeviceID(���ֲ�Ʒ���)
    /// </summary>
	/// <param name="line">�����߱���</param>
	/// <param name="station">���빤��վ</param>
	/// <param name="type">��Ʒ���ö��ֵ</param>
    /// <returns>��ȡ����DeviceID��ʧ���򷵻ؿ��ַ���</returns>
    char* ObtainDeviceIDwithType(const char* line, const char* station, unsigned int type);
   
    /// <summary>
    /// T10���������ϴ�(RF)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT10Identity idn, CT10Parameter para[], int paraLen);

    /// <summary>
    /// T20���������ϴ�(����·��)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT20Identity idn, CT20Parameter para[], int paraLen);

    /// <summary>
    /// T30���������ϴ�(дMAC)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT30Identity idn, CT30Parameter para);

    /// <summary>
    /// T40���������ϴ�(����������)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT40Identity idn, CT40Parameter para[], int paraLen);

	/// <summary>
    /// T50���������ϴ�(ADSL���ܲ���)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT50Identity idn, CT50Parameter para);

	/// <summary>
    /// T60���������ϴ�(PLC�������ܲ���)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT60Identity idn, CT60Parameter para);

	/// <summary>
    /// T70���������ϴ�(DECT����)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT70Identity idn, CT70Parameter para[], int paraLen);

	/// <summary>
    /// T80���������ϴ�(POE���ʲ���)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT80Identity idn, CT80Parameter para);

	/// <summary>
    /// T99���������ϴ�(���������Լ��)
    /// </summary>
    /// <param name="idn">identity����</param>
    /// <param name="para">parameter�������ݲ���</param>
    /// <returns>WebService����ֵ</returns>
    int DataUpload(CT99Identity idn, CT99Parameter para);

	/// <summary>
    /// ��鴫���DeviceID�Ƿ�Ϸ�
    /// </summary>
    /// <param name="devID">������DeviceID</param>
    /// <returns>true���Ϸ��� false�����Ϸ�</returns>
	bool CheckDevID(const char* devID);

	/// <summary>
    /// ��ȡ��ҵվ���ݽṹ��ֵ�������ȵĵ�һ�������ֶ�����
    /// </summary>
	char* GetDataLenError();

	/// <summary>
    /// �ͷ���Դ
    /// </summary>
	void Release();

private:
	char* mServerIP;
	int mConnTimeout;
};

#ifdef USE_DEVID_POOL
//----------Device ID Pool----------
// This class is exported from the WSDataTransCpp.dll
class WSDATATRANSCPP_API CDevIDPoolCpp {
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="line">��ǰ����</param>
	/// <param name="station">��ǰ��λ��</param>
	/// <param name="serverIP">������IP��ַ</param>
	/// <param name="wsTimeout">WebService��ʱʱ��</param>
	CDevIDPoolCpp(char* line, char* station, char* serverIP, int wsTimeout=5000);
    
	/// <summary>
	/// ��ȡ��ǰ��(����)��DeviceID������
	/// </summary>
	/// <returns>����</returns>
	int GetPoolCount();

	/// <summary>
	/// ��ʼ��DevID��
	/// </summary>
	/// <returns>true����ʼ���ɹ���false����ʼ��ʧ�ܣ��޷����ӷ�����������ȡ����ʣ���DevID</returns>
	bool InitDevIDPool();
       
	/// <summary>
	/// ��DevID���л�ȡһ��DevID
	/// </summary>
	/// <returns>�ַ�����ʽ��DeviceID,���Pool��Դ�ľ��򷵻ؿ�</returns>
	char* GetDevIDFromPool();
       
	/// <summary>
	/// �ͷ�DeviceID����Դ,��������д���ļ�
	/// </summary>
	/// <returns>true�������ɹ���false������ʧ��</returns>
	bool ReleaseDevIDPool();

	

private:
	char* mLine;
	char* mStation;
};

#endif