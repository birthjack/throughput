// XMLConfig.h: interface for the CXMLConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLCONFIG_H__E3645079_46B3_4790_AE8F_BEAAE9F33097__INCLUDED_)
#define AFX_XMLCONFIG_H__E3645079_46B3_4790_AE8F_BEAAE9F33097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"

using std::string;

class CXMLConfig
{
public:
	CXMLConfig(const char* szXmlFilename);
	~CXMLConfig();

	//����XPATH·����ȡ�ַ���
	string getXmlString(const char *szXpath);
    string bytestohexstring(char* bytes,int bytelength);
	int getXmlInt(const char* szXpath);

	bool writeXmlString(const string strValue, const char* szXpath);
	bool writeXmlInt(const int iValue, const char* szXpath);

	bool saveConfigFile();
	bool saveBakConfigFile();
	bool loadBakConfigFile();

private:
	//����ת��:��һ�ֱ���תΪ��һ�ֱ���
	int code_convert(char* from_charset, char* to_charset, char* inbuf,
					 int inlen, char* outbuf, int outlen);

	//UNICODE��תΪGB2312��
	char* u2g(char *inbuf);

	//GB2312��תΪUNICODE��
	char* g2u(char *inbuf);

	//����xpath��ѯ�ڵ㼯�ϣ��ɹ��򷵻�xpath�Ķ���ָ�룬ʧ�ܷ���NULL
	xmlXPathObjectPtr get_nodeset(const xmlChar *xpath);

	// ��ֹ�������캯����"="����
	CXMLConfig(const CXMLConfig&);
	CXMLConfig& operator=(const CXMLConfig&);
private:
	string m_strFilename;
	xmlDocPtr m_doc;
    xmlNodePtr curNodePtr;  //������ָ��
};

#endif // !defined(AFX_XMLCONFIG_H__E3645079_46B3_4790_AE8F_BEAAE9F33097__INCLUDED_)
