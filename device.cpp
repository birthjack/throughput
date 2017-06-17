/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   device.h

  Description: 定义设备的父类和目前的子类

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-09, liguoliang      create file.
*******************************************************************************/

#include "StdAfx.h"
#include "device.h"
#include "httpDeviceInfo.h"
#include "tddpDeviceInfo.h"
#include "TPWearableDeviceInfo.h"
#include "ADOConn.h"
#include "configure.h"
#include "testItems.h"
#include "WirelessTestDlg.h"
#include <afxinet.h>
#include <wincrypt.h>

#include "md5.h"
#include <string.h>

#define UNICODELENGTH 4
#define HEXLENGTH 16

//CString nonce;
//static int nCount = 1;

extern CMyDlg  *g_Dlg;

static char sKeyBuf[][32] = {"device_name","version_num","hardware_version","mac","IMEI",
							"meid","modemStatus","workingMode", "pin_status"};
// 获取基本信息统一使用的方法
char* Device::getIMEI()
{
	memset(imei, 0, sizeof(imei));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceImei(imei, sizeof(imei)))
	{
		strcpy(imei, "N/A");
	}

	return this->imei;
}

char* Device::getMEID()
{
	memset(meid, 0, sizeof(meid));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceMeid(meid, sizeof(meid)))
	{
		strcpy(meid, "N/A");
	}

	return this->meid;
}

char* Device::getSoftWareVersion()
{
	memset(sw_version, 0, sizeof(sw_version));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceSysSoftVer(sw_version, sizeof(sw_version)))
	{
		strcpy(sw_version, "N/A");
	}

	return this->sw_version;
}

char* Device::getHardWareVersion()
{
	memset(hw_version, 0, sizeof(hw_version));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceSysHardVer(hw_version, sizeof(hw_version)))
	{
		strcpy(hw_version, "N/A");
	}

	return this->hw_version;
}

char* Device::getWorkingMode()
{
	memset(working_mode, 0, sizeof(working_mode));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceWorkingMode(working_mode, sizeof(working_mode)))
	{
		strcpy(working_mode, "N/A");
	}

	return this->working_mode;
}

char* Device::getModemStatus()
{
	memset(modem_status, 0, sizeof(modem_status));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceModemStatus(modem_status, sizeof(modem_status)))
	{
		strcpy(modem_status, "N/A");
	}

	return modem_status;
}

char* Device::getPIN()
{
	memset(pin, 0, sizeof(pin));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDevicePin(pin, sizeof(pin)))
	{
		strcpy(pin, "N/A");
	}

	return pin;
}

char* Device::getMAC()
{
	memset(mac, 0, sizeof(mac));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceMac(mac, sizeof(mac)))
	{
		strcpy(mac, "N/A");
	}

	return mac;
}

// MIC devid 获取
char* Device::getDevid()
{
	memset(devid, 0, sizeof(devid));

	if (0 == initial)
	{
		if(deviceInfoInit(CON_DUTIP, CON_DUTPASSWORD,5))
		{
			initial = 1;
		} else
		{
			initial = 0;
		}
	}

	if (initial == 0 || !getDeviceId(devid, sizeof(devid)))
	{
		strcpy(devid, "N/A");
	}

	return devid;
}

//****************************7系列获取设备信息方法*********************************
int Device_7s::initialInfoItem()
{
	CString send_data;
	char csrf_str[16];
	int ret = 0;

	char cookies[50];
	memset(cookies, 0, sizeof(cookies));

	strcpy(cookies, "Basic%20YWRtaW46YWRtaW4%3D");

	send_data.Format("%s%s%s%s%s%s%s%s%s%s",
		"GET /cgi-bin/index.cgi HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	if (FALSE == getCSRFToken(csrf_str))
	{
		return FALSE;
	}

	// 发送能够从返回数据中解析出设备信息的请求
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /cgi-bin/system-tool-deviceinfo.cgi?CSRFToken=",
		csrf_str,
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\nReferer: http://",
		CON_DUTIP,
		"\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=0\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return 0;
	}
	return 1;
}
char* Device_7s::getSoftwareVerOld()
{
	getDeviceInfo_7s(sw_version, sizeof(sw_version), 2);

	return this->sw_version;
}

char* Device_7s::getHardwareVerOld()
{
	getDeviceInfo_7s(hw_version, sizeof(hw_version), 3);

	return this->hw_version;
}

char* Device_7s::getMACOld()
{
	getDeviceInfo_7s(mac, sizeof(mac), 4);

	return this->mac;
}

char* Device_7s::getIMEIOld()
{
	getDeviceInfo_7s(imei, sizeof(imei), 5);

	return this->imei;
}

int Device_7s::reset()
{
	CString send_data;
	char csrf_str[16];
	send_data.Format("%s%s%s%s%s%s%s%s%s%s",
		"GET /cgi-bin/index.cgi HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	if (FALSE == getCSRFToken(csrf_str))
	{
		return FALSE;
	}

	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /cgi-bin/system-tool-restore-reboot.cgi?CSRFToken=",
		csrf_str,
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\nReferer: http://",
		CON_DUTIP,
		"/cgi-bin/system-tool-restore.cgi?CSRFToken=",
		csrf_str,
		"\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");


	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	int reset_timeOut = getTimeOut(CON_RESET_TIME);

	return reset_timeOut;
}

//****************************8系列获取设备信息方法*********************************
int Device_8s::initialInfoItem()
{
	CString send_data;
	char seesion_id[16];
	int ret = 0;

	char cookies[50];
	memset(cookies, 0, sizeof(cookies));

	strcpy(cookies, "Basic%20YWRtaW46YWRtaW4%3D");

	// 发送能够从返回数据中解析出session_id的请求
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET / HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		//return 0;
	}

	if (FALSE == getSessionID(seesion_id))
	{
		strcpy(cookies, "Basic%20YWRtaW46MTIzNDU2");
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
			"GET / HTTP/1.1\r\n",
			"Host: ",
			CON_DUTIP,
			"\r\nConnection: Keep-Alive\r\n",
			"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
			"Accept-Encoding: gzip,deflate,sdch\r\n",
			"Accept-Language: zh-CN,zh;q=0.8\r\n",
			"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
			"Cookie: Authorization=",
			cookies,
			"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

		if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
		{
			//return 0;
		}
		if (FALSE == getSessionID(seesion_id))
		{
			strcpy(seesion_id, "111111");
			//return 0;
		}
	}

	// 发送修改命令请求
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /loginRpm/setPassword.htm?OK=1&password=123456&session_id=",
		seesion_id,
		" HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		//return FALSE;
	}

	if (TRUE == PageError())
	{
		strcpy(cookies, "Basic%20YWRtaW46YWRtaW4%3D");
		CON_SET_NEW_PASSWORD = 0;
	} else
	{
		strcpy(cookies, "Basic%20YWRtaW46MTIzNDU2");
		CON_SET_NEW_PASSWORD = 1;
	}

	// 发送能够从返回数据中解析出设备信息的请求
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/deviceInfo.htm?dataRequestOnly=1&session_id=",
		seesion_id,
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\nReferer: http://",
		CON_DUTIP,
		"\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=0\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		//return 0;
	}

	if (TRUE == PassWordWrong())
	{
		strcpy(cookies, "Basic%20YWRtaW46MTIzNDU2");
		// 发送能够从返回数据中解析出设备信息的请求
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			"GET /userRpm/deviceInfo.htm?dataRequestOnly=1&session_id=",
			seesion_id,
			" HTTP/1.1\r\nHost: ",
			CON_DUTIP,
			"\r\nConnection: Keep-Alive\r\nReferer: http://",
			CON_DUTIP,
			"\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
			"Accept-Encoding: gzip,deflate,sdch\r\n",
			"Accept-Language: zh-CN,zh;q=0.8\r\n",
			"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
			"Cookie: tLargeScreenP=1; Authorization=",
			cookies,
			"; subType=pcSub; TPLoginTimes=0\r\n\r\n");

		if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
		{
			//return 0;
		}
	}
	return 1;
}
char* Device_8s::getSoftwareVerOld()
{
	getDeviceInfo_8s(sw_version, sizeof(sw_version), 2);

	return this->sw_version;
}

char* Device_8s::getHardwareVerOld()
{
	getDeviceInfo_8s(hw_version, sizeof(hw_version), 3);

	return this->hw_version;
}

char* Device_8s::getMACOld()
{
	getDeviceInfo_8s(mac, sizeof(mac), 4);

	return this->mac;
}

char* Device_8s::getIMEIOld()
{
	getDeviceInfo_8s(imei, sizeof(imei), 5);

	return this->imei;
}

int Device_8s::reset()
{
	CString send_data;
	char seesion_id[16];
	char cookies[50];

	memset(cookies, 0, sizeof(cookies));
	memset(seesion_id, 0, sizeof(seesion_id));

	strcpy(cookies, "Basic%20YWRtaW46YWRtaW4%3D");
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET / HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	if (FALSE == getSessionID(seesion_id))
	{
		strcpy(cookies, "Basic%20YWRtaW46MTIzNDU2");
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
			"GET / HTTP/1.1\r\n",
			"Host: ",
			CON_DUTIP,
			"\r\nConnection: Keep-Alive\r\n",
			"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
			"Accept-Encoding: gzip,deflate,sdch\r\n",
			"Accept-Language: zh-CN,zh;q=0.8\r\n",
			"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
			"Cookie: Authorization=",
			cookies,
			"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

		if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
		{
			return FALSE;
		}
		if (FALSE == getSessionID(seesion_id))
		{
			return FALSE;
		}
	}

	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/restoreFactory.htm?session_id=",
		seesion_id,
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\nReferer: http://",
		CON_DUTIP,
		"/userRpm/restoreFactory.htm?session_id=",
		seesion_id,
		"\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:17.0) Gecko/17.0 Firefox/17.0\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=",
		cookies,
		"; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	int reset_timeOut = getTimeOut(CON_RESET_TIME);

	return reset_timeOut;
}

//****************************TR961 CMCC 获取设备信息方法***************************
int Device_lte_cmcc::initialInfoItem()
{
	CString sendBuf;
	CString sendBuf_login;
	CString sendBuf_login2;
	int nTimeout = 5;
	char *ptr;
	char tempStr[64],resultStr[64*UNICODELENGTH];

	sendBuf_login.Format("%s%s%s%s%s%s%s%s%s%s%s",
		"GET /login.cgi?_=1420506370678",
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: keep-alive\r\n",
		"Accept:*/*\r\n",
		"Cookie:tpweb_isMobile=false; nCount=2\r\n",
		"Expires:-1\r\n",
		"Pragma:no-cache",
		"Cache-Control:no-store, no-cache, must-revalidate\r\n",
		"User-Agent:Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n",
		"X-Requested-With:XMLHttpRequest\r\n\r\n");

	int ret = 0;
	//CString nonce;
	//static int nCount = 1;
	if (sendWebData(CON_DUTIP, sendBuf_login, nTimeout) && TRUE == getNonce())
	{
		 //认证预处理
		CString auth_field;     // 第二步认证需要的认证字段

		int i;
		CString tmpMd5code;
		CString authRealm = "Highwmg";
		CString authQop = "auth";
		CString HA1;
		CString HA2;
		CString authCnonce_f = "412909c008fba3b3";

		unsigned char encrypt1[] = "admin:Highwmg:admin";
		unsigned char encrypt2[] = "GET:/cgi/protected.cgi";
		unsigned char encrypt3[] = "GET:/cgi/xml_action.cgi";
		unsigned char decrypt[HEXLENGTH];


		CString authCount;
		char nCountHex[9];

		CString digestRes;

		MD5_CTX md5_HA1;
		MD5Init(&md5_HA1);

		//HA1
		MD5Update(&md5_HA1,encrypt1,strlen((char *)encrypt1));
		MD5Final(&md5_HA1,decrypt);

		for(i=0;i<HEXLENGTH;i++)
		{
			tmpMd5code.Format("%02x",decrypt[i]);
			HA1 = HA1 + tmpMd5code;
		}

		//HA2
		MD5_CTX md5_HA2;
		MD5Init(&md5_HA2);
		MD5Update(&md5_HA2,encrypt2,strlen((char *)encrypt2));
		MD5Final(&md5_HA2,decrypt);

		for(i=0;i<HEXLENGTH;i++)
		{
			tmpMd5code.Format("%02x",decrypt[i]);
			HA2 = HA2 + tmpMd5code;
		}

		//digestRes
		CString  dig = HA1 + ":" + nonce + ":" + "00000001" + ":" + authCnonce_f  + ":" + authQop + ":"+ HA2;
		unsigned char *puc = (unsigned char*)dig.GetBuffer(dig.GetLength());
		MD5_CTX md5_digestRes;
		MD5Init(&md5_digestRes);
		MD5Update(&md5_digestRes,puc,strlen((char *)puc));
		MD5Final(&md5_digestRes,decrypt);
		dig.ReleaseBuffer();

		for(i=0;i<HEXLENGTH;i++)
		{
			tmpMd5code.Format("%02x",decrypt[i]);
			digestRes = digestRes + tmpMd5code;
		}

		// 认证处理
		authentication_lte_cmcc(nonce, nCount, 0, auth_field);

		sendBuf_login2.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /login.cgi?Action=Digest&username=admin&realm=Highwmg&nonce=",
		nonce.GetBuffer( nonce.GetLength()),
		"&response=",
		digestRes.GetBuffer( digestRes.GetLength()),
		"&qop=auth&cnonce=",
		authCnonce_f.GetBuffer( authCnonce_f.GetLength()),
		"&temp=marvell&_=1420506370679",
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: keep-alive\r\n",
		auth_field.GetBuffer(auth_field.GetLength()),
		"\"\r\nCache-Control: no-store, no-cache, must-revalidate\r\n",
		"Accept:*/*\r\n",
		"Expires:-1\r\n",
		"Pragma:no-cache\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n\r\n");
	}

	if (sendWebData(CON_DUTIP, sendBuf_login2, nTimeout))
	{
		/* GET请求需要的认证字段 */
		CString auth_field_GetDevice;  // 在获取设备信息时需要的认证字段
		authentication_lte_cmcc(nonce, nCount, 0, auth_field_GetDevice);

		sendBuf.Format("%s%s%s%s%s%s%s",
			"GET /xml_action.cgi?method=get&module=duster&file=device_info",
			" HTTP/1.1\r\nHost: ",
			CON_DUTIP,
			"\r\nConnection: keep-alive\r\n",
			auth_field_GetDevice.GetBuffer(auth_field_GetDevice.GetLength()),
			"\"\r\nCache-Control: max-age=0\r\n",
			"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n\r\n");
			//"Cookie: subType=pcSub\r\n\r\n");

		return sendWebData(CON_DUTIP, sendBuf, nTimeout);

	}
	else
	{
		return FALSE;
	}
}

char* Device_lte_cmcc::getSoftwareVerOld()
{
	getDeviceInfo_lte_cmcc(sKeyBuf[1], sw_version, sizeof(sw_version));
	return this->sw_version;
}

char* Device_lte_cmcc::getHardwareVerOld()
{
	getDeviceInfo_lte_cmcc(sKeyBuf[2], hw_version, sizeof(hw_version));
	return this->hw_version;
}

char* Device_lte_cmcc::getMACOld()
{
	getDeviceInfo_lte_cmcc(sKeyBuf[3], mac, sizeof(mac));
	return this->mac;
}

char* Device_lte_cmcc::getIMEIOld()
{
	getDeviceInfo_lte_cmcc(sKeyBuf[4], imei, sizeof(imei));

	// 增加对16位异常IMEI的处理
	if (16 == strlen(this->imei))
	{
		memset(this->imei, 0 ,sizeof(this->imei));
		strcpy(this->imei, "N/A");
	}

	return this->imei;
}

int Device_lte_cmcc::getNonce()
{
	char tmp_nonce[32];
	int ret;

	ret = getDeviceNonce(tmp_nonce, sizeof(tmp_nonce));
	nonce = tmp_nonce;

	return ret;
}

int Device_lte_cmcc::SetBandTo40(int channel, int bandwidth)
{
	return fixedBand(CON_DUTIP, 5, nonce, nCount, channel, bandwidth);
}

int Device_lte_cmcc::reset()
{
	CString sendBuf;
	CString auth_field;

	// 认证处理
	authentication_lte_cmcc(nonce, nCount, 0, auth_field);

	sendBuf.Format("%s%s%s%s%s%s%s",
				"GET /xml_action.cgi?method=get&module=duster&file=restore_defaults",
				" HTTP/1.1\r\nHost: ",
				CON_DUTIP,
				"\r\nConnection: keep-alive\r\n",
				auth_field.GetBuffer(auth_field.GetLength()),
				"\"\r\nAccept-Language:zh-CN,zh;q=0.8\r\n",
				"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n\r\n");

	return sendWebData(CON_DUTIP, sendBuf, 5);

}
//****************************路由+3G系列获取设备信息方法***************************
int Device_RouterAnd3G::reset()
{
	/************************************************************************/
	/* 路由+3G产品的重置针对内销和外销会有不一样                                   */
	/************************************************************************/

	CString send_data;
	char seesion_id[16];
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET / HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\nAuthorization: Basic YWRtaW46YWRtaW4=\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	if (FALSE == getSessionID(seesion_id))
	{
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/RestoreDefaultCfgRpm.htm?RestoreFactory=\"恢复出厂设置\"  HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\nCache-Control: max-age=0\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");
	} else
	{
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/RestoreDefaultCfgRpm.htm?session_id=",
		seesion_id,
		"&Restorefactory=Restore HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/userRpm/RestoreDefaultCfgRpm.htm?session_id=",
		seesion_id,
		"\r\nAuthorization: Basic YWRtaW46YWRtaW4=\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: Authorization=Basic%20YWRtaW46YWRtaW4%3D; subType=pcSub; TPLoginTimes=1\r\n\r\n");
	}

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	int reset_timeOut = getTimeOut(CON_RESET_TIME);

	return reset_timeOut;
}


//****************************路由系列获取设备信息方法*******************************
int Device_Router::reset()
{
	CString send_data;
	char seesion_id[16];

	/************************************************************************/
	/* 路由产品的重置需要针对具体产品在web请求内容上做微调                           */
	/************************************************************************/


	// 发送修改密码命令
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/ChangeDefaultPwdRpm.htm?newpassword=123456&newpassword2=123456&Save=Save HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46MTIzNDU2; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}


	// 发送获取session_id命令
	send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET / HTTP/1.1\r\n",
		"Host: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46MTIzNDU2; subType=pcSub; TPLoginTimes=1\r\n\r\n");

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	// 发送恢复出厂设置命令
	if (FALSE == getSessionID(seesion_id))
	{
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/RestoreDefaultCfgRpm.htm?RestoreFactory=\"恢复出厂设置\"  HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\nCache-Control: max-age=0\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46MTIzNDU2; subType=pcSub; TPLoginTimes=1\r\n\r\n");
	} else
	{
		send_data.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"GET /userRpm/RestoreDefaultCfgRpm.htm?RestoreFactory=Restore&seesion_id=",
		seesion_id,
		" HTTP/1.1\r\nHost: ",
		CON_DUTIP,
		"\r\nConnection: Keep-Alive\r\n",
		"Referer: http://",
		CON_DUTIP,
		"/\r\n",
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.112 Safari/534.30\r\n",
		"Accept: text/css,*/*;q=0.1\r\n",
		"Accept-Encoding: gzip,deflate,sdch\r\n",
		"Accept-Language: zh-CN,zh;q=0.8\r\n",
		"Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n",
		"Cookie: tLargeScreenP=1; Authorization=Basic%20YWRtaW46MTIzNDU2; subType=pcSub; TPLoginTimes=1\r\n\r\n");
	}

	if (FALSE == sendWebData(CON_DUTIP,send_data, 5))
	{
		return FALSE;
	}

	int reset_timeOut = getTimeOut(CON_RESET_TIME);

	return reset_timeOut;
}


//****************************LTE MiFi系列获取设备信息方法*******************************
CString Device_lte::SendHttpPostRequest(const CString &url,
										const CString &mod,
										const CString &data)
{
	CString szData,szAllData;

	try {
		CInternetSession cis;
		CHttpConnection *conn =
			cis.GetHttpConnection((LPCTSTR)CON_DUTIP,
								(INTERNET_PORT)80);

		CHttpFile *pF = conn->OpenRequest(CHttpConnection::HTTP_VERB_POST,
										mod);
		pF->AddRequestHeaders("Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n");
		pF->AddRequestHeaders("Accept: application/json, text/javascript, */*; q=0.01");
//		CHttpFile* pF=(CHttpFile*)cis.OpenURL(url);
		CString strHeaders;
		//strHeaders.Format("%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s",
		//			"Host: 192.168.0.1",
		//			"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0",
		//			"Accept: application/json, text/javascript, */*; q=0.01",
		//			"Accept-Language: zh-cn,en-us;q=0.8,ja;q=0.5,en;q=0.3",
		//			"Accept-Encoding: gzip, deflate",
		//			"Content-Type: application/x-www-form-urlencoded; charset=UTF-8",
		//			"X-Requested-With: XMLHttpRequest",
		//			"Referer: http://192.168.0.1/login.html?kickedOut",
		//			"Content-Length: 30",
		//			"Connection: keep-alive",
		//			"Pragma: no-cache",
		//			"Cache-Control: no-cache");

		BOOL result = pF->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)data, data.GetLength());

		if(result) {
			//AfxMessageBox(_T("请求成功"));
		}

		while(pF->ReadString(szData)) {
			//读取文件
			szAllData+="\r\n";
			szAllData+=szData;
		}
		pF->Close();
		cis.Close();
		//AfxMessageBox(szAllData);
		//AfxMessageBox(_T("获取数据成功"));
	} catch(CException *e) {
		AfxMessageBox(_T("LTE MiFi: Http请求失败"));
	}

	return szAllData;
}


CString Device_lte::GetHttpToken()
{
	CString token;
	//Authentication to get token
	//Step 1: Get Nonce
	CString url=CString("http://") + CString(CON_DUTIP);
	CString data;
	data.Format("{\"module\":\"authenticator\",\"action\":0}");
	CString ret = this->SendHttpPostRequest(url, "/cgi-bin/qcmap_auth/", data);

	char _nonce[64];
	_nonce[0] = '\0';
	CString nonce_pattern("nonce");
	int a = ret.Find((LPCTSTR)nonce_pattern);
	if (a >= 0) {
		int pos = a + nonce_pattern.GetLength();
		while (ret.GetAt(pos++) != '\"');

		while (ret.GetAt(pos++) != '\"');

		int n = 0;
		while (ret.GetAt(pos) != '\"' && n < sizeof(_nonce) - 1)
			_nonce[n++] = ret.GetAt(pos++);

		_nonce[n] = '\0';
	}

	if (strlen(_nonce) <= 0) {
		AfxMessageBox("Failed to get token for http req");
		return token;
	}

	//Step 2: Get digest value
	char tmp[512];
	sprintf(tmp, "%s:%s:%s",
			CON_DUTUSER,
			CON_DUTPASSWORD,
			_nonce);

	//Calculate md5
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, NULL, NULL,
							PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		AfxMessageBox("md5 error: get crypt ctx failed");
		return token;
	}

	HCRYPTHASH hHash;
	//Alg Id:CALG_MD5,CALG_SHA
	if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
		AfxMessageBox("md5 error: create hash failed");
		CryptReleaseContext(hProv, 0);
		return token;
	}

	if(!CryptHashData(hHash, (const BYTE *)tmp, strlen(tmp), 0)) {
		AfxMessageBox("md5 error: cal md5 failed");
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return token;
	}

	DWORD dwHashLen;
	DWORD dwLen = sizeof(dwHashLen);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwHashLen), &dwLen, 0);

	BYTE *pbOutHash = new BYTE[dwHashLen];
	if (pbOutHash == NULL) {
		AfxMessageBox("md5 error: allocate mem 1 failed");
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return token;
	}

	dwLen = dwHashLen;
	CryptGetHashParam(hHash, HP_HASHVAL, pbOutHash, &dwLen, 0);

	int md5_str_len = dwHashLen * 2 + 1;
	char *_dg = new char[md5_str_len];
	if (_dg == NULL) {
		AfxMessageBox("md5 error: allocate mem 2 failed");
		delete[] pbOutHash;
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return token;
	}

	DWORD ii = 0;
	for (ii = 0; ii < dwHashLen; ii++) {
		char h = pbOutHash[ii] >> 4;
		if (h >= 0 && h <= 9)
			h = '0' + h;
		else if (h >= 0xa && h <= 0xf)
			h = 'a' + h - 0xa;

		char l = pbOutHash[ii] & 0x0F;
		if (l >= 0 && l <= 9)
			l = '0' + l;
		else if (l >= 0xa && l <= 0xf)
			l = 'a' + l - 0xa;

		_dg[ii * 2] = h;
		_dg[ii * 2 + 1] = l;
	}

	_dg[ii * 2] = '\0';

	delete[] pbOutHash;
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	//Step 3: Get token
	CString token_url =CString("http://") + CString(CON_DUTIP);
	CString token_data;
	token_data.Format("{\"module\":\"authenticator\",\"action\":1,\"digest\":\"%s\"}", _dg);
	CString token_ret = this->SendHttpPostRequest(url, "/cgi-bin/qcmap_auth/", token_data);

	delete[] _dg;

	char _token[128];
	_token[0] = '\0';
	CString token_pattern("token");
	int b = token_ret.Find(token_pattern);
	if (b >= 0) {
		int pos = b + token_pattern.GetLength();
		while (token_ret.GetAt(pos++) != '\"');

		while (token_ret.GetAt(pos++) != '\"');

		int p = 0;
		while (token_ret.GetAt(pos) != '\"' && p < sizeof(_token) - 1)
			_token[p++] = token_ret.GetAt(pos++);

		_token[p] = '\0';
	}

	return CString(_token);
}


static CString Current_WiFi_Config;

int Device_lte::reset()
{
	CString token = this->GetHttpToken();
	if (token.IsEmpty()) {
		AfxMessageBox("RESET: Failed to get token");
		return -1;
	}

	CString url =CString("http://") + CString(CON_DUTIP);
	CString data;
	data.Format("{\"module\":\"restoreDefaults\",\"action\":0,\"token\":\"%s\"}",token);
	CString ret = this->SendHttpPostRequest(url, "/cgi-bin/qcmap_web_cgi/", data);

	this->band = BAND_INVALID;

	return 0;
}


enum WiFiBand Device_lte::getWiFiBand()
{
	if (this->band != BAND_INVALID)
		return this->band;

	enum WiFiBand wifi_band = BAND_INVALID;

	// 1. Get token
	CString token = this->GetHttpToken();
	if (token.IsEmpty()) {
		AfxMessageBox("Get Band: Failed to get token");
		return wifi_band;
	}

	// 2. Get WiFi status
	CString url =CString("http://") + CString(CON_DUTIP);// + CString("cgi-bin/qcmap_web_cgi");
	CString data;
	data.Format("{\"module\":\"wlan\",\"action\":0,\"token\":\"%s\"}",token);
	CString ret = this->SendHttpPostRequest(url, "/cgi-bin/qcmap_web_cgi/", data);

	// 3. Analyze WiFi Band info
	char band[16];
	band[0] = '\0';
	CString band_pattern("bandType");
	int p = ret.Find(band_pattern);
	if (p >= 0) {
		int pos = p + band_pattern.GetLength();

		while (ret.GetAt(pos++) != '\"');

		while (ret.GetAt(pos) < '0' || ret.GetAt(pos) > '9')
			pos++;

		int n = 0;
		while (ret.GetAt(pos) >= '0' && ret.GetAt(pos) <= '9'
				&& n < sizeof(band) - 1)
			band[n++] = ret.GetAt(pos++);

		band[n] = '\0';
	}

	if (strlen(band) > 0) {
		int b = atoi(band);
		if (b == 0)
			wifi_band = BAND_2_GHZ;
		else if (b == 1)
			wifi_band = BAND_5_GHZ;
	}

	Current_WiFi_Config = ret;
	this->band = wifi_band;

	return wifi_band;
}

void Device_lte::setWiFiBand(enum WiFiBand band)
{
	this->band = BAND_INVALID;

	this->getWiFiBand();

	int b;
	char _channel[8];
	char _reg[8];
	char region[8];
	int wireless_mode = -1;
	int insert_pos = -1;

	// 2. Change WiFI config
	CString wifi_conf = Current_WiFi_Config;

	// Get region to decide channel to set
	CString reg_pattern("region");
	int ppp = wifi_conf.Find(reg_pattern);
	if (ppp >= 0) {
		int pos = ppp + reg_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != '\"');

		while (wifi_conf.GetAt(pos++) != '\"');

		// Get reg
		int v_pos = 0;
		while (wifi_conf.GetAt(pos + v_pos) != '\"' && v_pos < sizeof(_reg)) {
			_reg[v_pos] = wifi_conf.GetAt(pos + v_pos);
			v_pos++;
		}
		_reg[v_pos] = '\0';
	}

	if (band == BAND_2_GHZ) {
		b = 0;
		wireless_mode = 5;

		strcpy(_channel, LTE_MIFI_TEST_CHANNEL_2GHZ);

		if (strcmp(_reg, LTE_MIFI_7350_REG) == 0
			|| strcmp(_reg, LTE_MIFI_7350_WEB_REG_DEFAULT) == 0)
			strcpy(region, LTE_MIFI_7350_REG);
		else if (strcmp(_reg, LTE_MIFI_961_REG) == 0)
			strcpy(region, LTE_MIFI_961_REG);
		else
			return;
	} else if (band == BAND_5_GHZ) {
		b = 1;
		wireless_mode = 6;

		if (strcmp(_reg, LTE_MIFI_7350_REG) == 0
			|| strcmp(_reg, LTE_MIFI_7350_WEB_REG_DEFAULT) == 0) {
			strcpy(region, LTE_MIFI_7350_REG);
			strcpy(_channel, LTE_MIFI_7350_TEST_CHANNEL_5GHZ);
		} else if (strcmp(_reg, LTE_MIFI_961_REG) == 0) {
			strcpy(region, LTE_MIFI_961_REG);
			strcpy(_channel, LTE_MIFI_961_TEST_CHANNEL_5GHZ);
		} else {
			return;
		}
	} else {
		return;
	}

	// Set Region
	CString region_pattern("region");
	int rrrr = wifi_conf.Find(region_pattern);
	if (rrrr >= 0) {
		int pos = rrrr + region_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != ':');

		int start = pos;

		while (wifi_conf.GetAt(pos++) != ',');

		int end = pos;
		if (end > start) {
			CString region_para;
			region_para.Format("\"%s\",", region);
			wifi_conf.Delete(start, end - start);
			wifi_conf.Insert(start, region_para);
		}
	}

	// Set Band
	CString band_pattern("bandType");
	int p = wifi_conf.Find(band_pattern);
	if (p >= 0) {
		int pos = p + band_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != ':');

		int start = pos;

		while (wifi_conf.GetAt(pos++) != ',');

		int end = pos;
		if (end > start) {
			CString band_para;
			band_para.Format("%d,", b);
			wifi_conf.Delete(start, end - start);
			wifi_conf.Insert(start, band_para);
		}
	}

	// Set Channel
	CString chan_pattern("channel");
	int pppp = wifi_conf.Find(chan_pattern);
	if (pppp >= 0) {
		int pos = pppp + chan_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != ':');

		int start = pos;
		int end;

		while (wifi_conf.GetAt(pos++) != ',');

		end = pos;
		if (end > start) {
			CString channel_para;
			channel_para.Format("%s,", _channel);
			wifi_conf.Delete(start, end - start);
			wifi_conf.Insert(start, channel_para);
		}
	}

	// Set Wireless mode
	if (wireless_mode < 0)
		return;
	CString wm_pattern("wirelessMode");
	int ppppp = wifi_conf.Find(wm_pattern);
	if (ppppp >= 0) {
		int pos = ppppp + wm_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != ':');

		int start = pos;
		int end;

		while (wifi_conf.GetAt(pos++) != ',');

		end = pos;
		if (end > start) {
			CString mode_para;
			mode_para.Format("%d,", wireless_mode);
			wifi_conf.Delete(start, end - start);
			wifi_conf.Insert(start, mode_para);
		}
	}

	// 2. Get token
	CString token = this->GetHttpToken();
	if (token.IsEmpty()) {
		AfxMessageBox("Set Band: Failed to get token");
		return;
	}

	// Set Action
	CString act_pattern("action");
	int pp = wifi_conf.Find(act_pattern);
	if (pp >= 0) {
		int pos = pp + act_pattern.GetLength();

		while (wifi_conf.GetAt(pos++) != ':');

		int start = pos;

		while (wifi_conf.GetAt(pos++) != ',');

		int end = pos;
		if (end > start) {
			CString act_para;
			act_para.Format("%d,", 1);
			wifi_conf.Delete(start, end - start);
			wifi_conf.Insert(start, act_para);

			start += act_para.GetLength();
			// Insert paras here.
			// Paras include module, action, token, etc.
			wifi_conf.Insert(start, "\"module\":\"wlan\",");
			CString token_para;
			token_para.Format("\"token\":\"%s\",", token);
			wifi_conf.Insert(start, token_para);
		}
	} else {
		insert_pos = wifi_conf.Find('{', 0);
		if (insert_pos < 0)
			return;

		insert_pos++;

		// Insert paras here.
		// Paras include module, action, token, etc.
		wifi_conf.Insert(insert_pos, "\"module\":\"wlan\",");
		wifi_conf.Insert(insert_pos, "\"action\":1,");
		CString token_para;
		token_para.Format("\"token\":\"%s\",", token);
		wifi_conf.Insert(insert_pos, token_para);
	}

	// 3. Send request to set WiFi Band
	CString url =CString("http://") + CString(CON_DUTIP);// + CString("cgi-bin/qcmap_web_cgi");
	CString ret = this->SendHttpPostRequest(url, "/cgi-bin/qcmap_web_cgi/", wifi_conf);

	this->band = BAND_INVALID;

	return;
}

//****************************TPWearable*******************************
char* DeviceTPWearable::getIMEI()
{
	memset(imei, 0, sizeof(imei));
	strcpy(imei, "N/A");
	return imei;
}

char* DeviceTPWearable::getMEID()
{
	memset(meid, 0, sizeof(meid));
	strcpy(meid, "N/A");
	return meid;
}

char* DeviceTPWearable::getSoftWareVersion()
 {
	memset(sw_version, 0, sizeof(sw_version));

	if (0 == initial)
	{
		if (GetTPWearableInfo(CON_DUTIP))
		{
			initial = 1;
		}
		else
		{
			initial = 0;
		}
	}

	if ((0 == initial) || (0 == GetTPWearableSoftVer(sw_version)))
	{
		strcpy(sw_version, "N/A");
	}

	return sw_version;
}

char* DeviceTPWearable::getHardWareVersion()
{
	memset(hw_version, 0, sizeof(hw_version));

	if (0 == initial)
	{
		if (GetTPWearableInfo(CON_DUTIP))
		{
			initial = 1;
		}
		else
		{
			initial = 0;
		}
	}

	if ((0 == initial) || (0 == GetTPWearableHardVer(hw_version)))
	{
		strcpy(hw_version, "N/A");
	}

	return hw_version;
}

char* DeviceTPWearable::getPIN()
{
	memset(pin, 0, sizeof(pin));
	strcpy(pin, "N/A");
	return pin;
}

char* DeviceTPWearable::getMAC()
{
	memset(mac, 0, sizeof(mac));

	if (0 == initial)
	{
		if (GetTPWearableInfo(CON_DUTIP))
		{
			initial = 1;
		}
		else
		{
			initial = 0;
		}
	}

	if ((0 == initial) || (0 == GetTPWearableMac(mac)))
	{
		strcpy(mac, "N/A");
	}

	return mac;
}

char* DeviceTPWearable::getDevid()
{
	memset(devid, 0, sizeof(devid));

	if (0 == initial)
	{
		if (GetTPWearableInfo(CON_DUTIP))
		{
			initial = 1;
		}
		else
		{
			initial = 0;
		}
	}

	if ((0 == initial) || (0 == GetTPWearableDeviceId(devid)))
	{
		strcpy(devid, "N/A");
	}

	return devid;
}


char* DeviceTPWearable::getWorkingMode()
{
	memset(working_mode, 0, sizeof(working_mode));
	strcpy(working_mode, "N/A");
	return working_mode;
}

char* DeviceTPWearable::getModemStatus()
{
	memset(modem_status, 0, sizeof(modem_status));
	strcpy(modem_status, "N/A");
	return modem_status;
}

int DeviceTPWearable::reset()
{
	if (ResetTPWDevice(CON_DUTIP))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//****************************720pCarDv*******************************
double DeviceCarDv:: startIperfPC(const char *test_in_pc)
{
	char l_shellResultBuff[256];
	FILE *pCommandFD;
	//int number = 0;  //-1
	int line = 0;
	double result = 0.0;
	int try_times = 0;
	int max_try_times = 0;
	CString str;

	memset(l_shellResultBuff, 0, sizeof(l_shellResultBuff));
	max_try_times = CON_TEST_TIME / 5;
	CButton *m_Avg = (CButton*)g_Dlg->GetDlgItem(IDC_AVG);

	if (test_in_pc == NULL)
	{
		return -1;
	}

	pCommandFD = _popen(test_in_pc, "r");
	if (pCommandFD == NULL)
	{
		return -1;
	}

	WaitForSingleObject(hMutex_iperf, INFINITE);
	if (IPERF_START_PC == runState.iperfStatus)
	{
		while (!fgets(l_shellResultBuff, sizeof(l_shellResultBuff), pCommandFD))
		{
			Sleep(1000);
		}

		runState.iperfStatus = IPERF_SWITCH_DUT;
	}
	else if (IPERF_SWITCH_PC == runState.iperfStatus)
	{
		runState.iperfStatus = IPERF_SWITCH_WRITE;
		g_Dlg->updateCurState(">>iperf开始运行");
	}
	ReleaseMutex(hMutex_iperf);


	while (runState.iperfStatus != IPERF_SWITCH_WRITE)
	{
		Sleep(1000);
	}

	myTimeOutThread = CreateThread(NULL, 0, timeOutThread, NULL, 0, NULL);

	while (fgets(l_shellResultBuff, sizeof(l_shellResultBuff), pCommandFD) && try_times < max_try_times)
	{
		if (strncmp(l_shellResultBuff, "[SUM]", strlen("[SUM]")) == 0)
		{
			double tmp;
			if (tmp = findThrIperfResult(l_shellResultBuff))
			{
				line++;
			}
			result += tmp;
			try_times++;

			str.Format("正在运行：已过 %d 秒", 5 * try_times);

			m_Avg->SetWindowText(str);
		}

	}

	if (line > 0)
	{
		result = result / line;
	}
	else
	{
		result = 0.0;
	}

 	_pclose(pCommandFD);

	system("TASKKILL /F /IM iperf.exe /T");

	runState.iperfStatus = IPERF_STOP;

	if (myTimeOutThread != NULL)
	{
        CloseHandle(myTimeOutThread);
        myTimeOutThread = NULL;
	}

	return result;
}

int DeviceCarDv:: startIperfDUT(const char *test_in_dut)
{
	if (StartTPWearableIperf(test_in_dut, CON_DUTIP))
	{
		if (IPERF_START_DUT == runState.iperfStatus)
		{
			runState.iperfStatus = IPERF_SWITCH_PC;
		}
		else if (IPERF_SWITCH_DUT == runState.iperfStatus)
		{
			runState.iperfStatus = IPERF_SWITCH_WRITE;
			g_Dlg->updateCurState(">>iperf开始运行");
		}
		else
		{
			return 1;
		}

		return 0;
	}

	return 1;
}


