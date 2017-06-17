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

	//根据XPATH路径读取字符串
	string getXmlString(const char *szXpath);
    string bytestohexstring(char* bytes,int bytelength);
	int getXmlInt(const char* szXpath);

	bool writeXmlString(const string strValue, const char* szXpath);
	bool writeXmlInt(const int iValue, const char* szXpath);

	bool saveConfigFile();
	bool saveBakConfigFile();
	bool loadBakConfigFile();

private:
	//代码转换:从一种编码转为另一种编码
	int code_convert(char* from_charset, char* to_charset, char* inbuf,
					 int inlen, char* outbuf, int outlen);

	//UNICODE码转为GB2312码
	char* u2g(char *inbuf);

	//GB2312码转为UNICODE码
	char* g2u(char *inbuf);

	//调用xpath查询节点集合，成功则返回xpath的对象指针，失败返回NULL
	xmlXPathObjectPtr get_nodeset(const xmlChar *xpath);

	// 禁止拷贝构造函数和"="操作
	CXMLConfig(const CXMLConfig&);
	CXMLConfig& operator=(const CXMLConfig&);
private:
	string m_strFilename;
	xmlDocPtr m_doc;
    xmlNodePtr curNodePtr;  //定义结点指针
};

#endif // !defined(AFX_XMLCONFIG_H__E3645079_46B3_4790_AE8F_BEAAE9F33097__INCLUDED_)
