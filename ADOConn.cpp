// ADOConn.cpp: implementation of the ADOConn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WirelessTest.h"
#include "ADOConn.h"
#include "configure.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ADOConn::ADOConn()
{

}

ADOConn::~ADOConn()
{

}

void ADOConn::OnInitADOConn()
{
   ::CoInitialize(NULL);
   try
	{
		//创建connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//设置连接字符串
		CString login;

		// [liguoliang start]
		//login.Format("Provider=SQLOLEDB;Persist Security Info=True;Driver=SQL Server;Server=%s\\%s;Database=%s;UID=%s;PWD=%s",CON_DATAIP,CON_DATASERVER, CON_DATANAME,CON_username,CON_password);
		//login.Format("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s;Network Address=%s",CON_password,CON_username,CON_DATANAME,CON_DATASERVER,CON_DATAIP);
		login.Format("Provider=SQLNCLI10;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s;Network Address=%s",CON_password,CON_username,CON_DATANAME,CON_DATASERVER,CON_DATAIP);
		// [liguoliang end]
		//AfxMessageBox(login);

		_bstr_t strConnect=login;
		//ADOConn::m_pConnection->Open(strConnect,CON_username,CON_password,adModeUnknown);
		ADOConn::m_pConnection->Open(strConnect,"","",adModeUnknown);
	}
	//捕捉异常
	catch(_com_error e)
	{
		//显示错误信息
		AfxMessageBox(e.ErrorMessage()/*e.Description()*/);
	}
}



_RecordsetPtr& ADOConn::GetRecordSet(_bstr_t bstrSQL)
{
   try
	{
		if(m_pConnection==NULL)
              OnInitADOConn();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
        m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
    catch(_com_error e)
	{
		e.Description();
	}
	return m_pRecordset;
}

BOOL ADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	_variant_t RecordsAffected;
	try
	{
        if(m_pConnection==NULL)
	     	OnInitADOConn();
        m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		e.Description();
		return false;
	}
}

void ADOConn::ExitConnect()
{
    if(m_pRecordset!=NULL)
	{
		m_pRecordset->Close();
		m_pRecordset.Release();
		m_pRecordset = NULL;
	}
	m_pConnection->Close();
	m_pConnection.Release();
	m_pConnection = NULL;
}
