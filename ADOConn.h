// ADOConn.h: interface for the ADOConn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADOCONN_H__0A254453_6FEE_4F17_A5A6_42017FD31082__INCLUDED_)
#define AFX_ADOCONN_H__0A254453_6FEE_4F17_A5A6_42017FD31082__INCLUDED_
//#import "C:\Special\msado15.dll"no_namespace \
//	rename("EOF","adoEOF")rename("BOF","adoBOF")
#import ".\msado15.dll"no_namespace \
	rename("EOF","adoEOF")rename("BOF","adoBOF")
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ADOConn  
{
public:
	void ExitConnect();
	BOOL ExecuteSQL(_bstr_t bstrSQL);
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
	void OnInitADOConn();
	ADOConn();
	virtual ~ADOConn();
	//添加一个指向Connection对象的指针
	_ConnectionPtr m_pConnection;
	//添加一个指向Recordset对象的指针
	_RecordsetPtr m_pRecordset;

};

#endif // !defined(AFX_ADOCONN_H__0A254453_6FEE_4F17_A5A6_42017FD31082__INCLUDED_)
