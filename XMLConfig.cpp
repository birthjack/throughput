// XMLConfig.cpp: implementation of the CXMLConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLConfig.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLConfig::CXMLConfig(const char* szXmlFilename)
{
	m_strFilename = szXmlFilename;
	//xmlNodePtr curNodePtr;  //定义结点指针

	m_doc = xmlReadFile(szXmlFilename,"UTF-8",XML_PARSE_RECOVER);  //解析文件
	if (m_doc == NULL)
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		xmlFreeDoc(m_doc);
		exit(1);
	}
	curNodePtr = xmlDocGetRootElement(m_doc);  //确定文档根元素

	/*检查确认当前文档中包含内容*/
	if (curNodePtr == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(m_doc);
		exit(1);
	}
}

CXMLConfig::~CXMLConfig()
{
	if (NULL != m_doc)
	{
		xmlFreeDoc(m_doc);
	}
}

int CXMLConfig::code_convert(char* from_charset, char* to_charset, char* inbuf,
				 int inlen, char* outbuf, int outlen)
{
	iconv_t cd;
	char** pin = &inbuf;
	char** pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if(cd == 0)
		return -1;
	memset(outbuf,0,outlen);
	if(iconv(cd,(const char**)pin,(unsigned int *)&inlen,pout,(unsigned int*)&outlen)
		== -1)
		return -1;
	iconv_close(cd);
	return 0;
}

char* CXMLConfig::u2g(char *inbuf)
{
	int nOutLen = 2 * strlen(inbuf) + 1;
	char* szOut = (char*)malloc(nOutLen);

	if (-1 == code_convert("utf-8","gb2312",inbuf,strlen(inbuf),szOut,nOutLen))
	{
		free(szOut);
		szOut = NULL;
	}
	return szOut;
}

char* CXMLConfig::g2u(char *inbuf)
{
	int nOutLen = 2 * strlen(inbuf) + 1;
	char* szOut = (char*)malloc(nOutLen);

	if (-1 == code_convert("gb2312","utf-8",inbuf,strlen(inbuf),szOut,nOutLen))
	{
		free(szOut);
		szOut = NULL;
	}
	return szOut;
}

xmlXPathObjectPtr CXMLConfig::get_nodeset(const xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(m_doc);
	if (context == NULL)
	{
		fprintf(stderr,"context is NULL\n");
		return NULL;
	}

	int status = xmlXPathRegisterNs(context,(const xmlChar *)"ns",
      (const xmlChar *)"http://www.microsoft.com/networking/WLAN/profile/v1");

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		fprintf(stderr,"xmlXPathEvalExpression return NULL\n");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		fprintf(stderr,"nodeset is empty\n");
		return NULL;
	}

	return result;
}

string CXMLConfig::getXmlString(const char *szXpath)
{
	xmlXPathObjectPtr result;
	string strRel;

	result = get_nodeset(BAD_CAST szXpath);

	if (result == NULL)
	{
		fprintf(stderr,"xmlXPathEvalExpression return NULL\n");
		return NULL;
	}

	xmlNodeSetPtr nodeSetPtr = result->nodesetval;

	//查出符合xpath的节点集合
	if (xmlXPathNodeSetIsEmpty(nodeSetPtr))
	{
		xmlXPathFreeObject(result);
		fprintf(stderr,"nodeset is empty\n");
		return NULL;
	}

	//查出节点集合中唯一节点的内容值
	xmlNodePtr nodePtr = nodeSetPtr->nodeTab[0];
	strRel = (const char*)xmlNodeGetContent(nodePtr);
	return strRel;
}

int CXMLConfig::getXmlInt(const char* szXpath)
{
	xmlXPathObjectPtr result;
	int iRel;

	result = get_nodeset(BAD_CAST szXpath);

	if (result == NULL)
	{
		fprintf(stderr,"xmlXPathEvalExpression return NULL\n");
		return NULL;
	}

	xmlNodeSetPtr nodeSetPtr = result->nodesetval;

	//查出符合xpath的节点集合
	if (xmlXPathNodeSetIsEmpty(nodeSetPtr))
	{
		xmlXPathFreeObject(result);
		fprintf(stderr,"nodeset is empty\n");
		return NULL;
	}

	xmlNodePtr nodePtr = nodeSetPtr->nodeTab[0];
	iRel = atoi((const char*)xmlNodeGetContent(nodePtr));
	return iRel;
}

bool CXMLConfig::writeXmlString(const string strValue, const char* szXpath)
{
	xmlXPathObjectPtr result;

	result = get_nodeset(BAD_CAST szXpath);
	if (result == NULL)
	{
		fprintf(stderr,"xmlXPathEvalExpression return NULL\n");
		return false;
	}

	xmlNodeSetPtr nodeSetPtr = result->nodesetval;

	//查出符合xpath的节点集合
	if (xmlXPathNodeSetIsEmpty(nodeSetPtr))
	{
		xmlXPathFreeObject(result);
		fprintf(stderr,"nodeset is empty\n");
		return false;
	}

	//查出节点集合中唯一节点的内容值
	xmlNodePtr nodePtr = nodeSetPtr->nodeTab[0];
	xmlNodeSetContent(nodePtr, BAD_CAST strValue.c_str());
	return true;
}

bool CXMLConfig::writeXmlInt(const int iValue, const char* szXpath)
{
	xmlXPathObjectPtr result;

	result = get_nodeset(BAD_CAST szXpath);

	if (result == NULL)
	{
		fprintf(stderr,"xmlXPathEvalExpression return NULL\n");
		return false;
	}

	xmlNodeSetPtr nodeSetPtr = result->nodesetval;

	//查出符合xpath的节点集合
	if (xmlXPathNodeSetIsEmpty(nodeSetPtr))
	{
		xmlXPathFreeObject(result);
		fprintf(stderr,"nodeset is empty\n");
		return false;
	}

	//查处节点集合中唯一节点的内容值
	xmlNodePtr nodePtr = nodeSetPtr->nodeTab[0];
	char* szBuffer =  new char[sizeof(int) + 1];
	itoa(iValue,szBuffer,10);
	xmlNodeSetContent(nodePtr, BAD_CAST szBuffer);
	delete szBuffer;
	return true;
}

bool CXMLConfig::saveConfigFile()
{
	if (m_doc == NULL || m_strFilename == "")
	{
		return false;
	}
	saveBakConfigFile();
	int nRel = xmlSaveFormatFileEnc(m_strFilename.c_str(),m_doc,"UTF-8",1);
	if (nRel == -1)
	{
		return false;
	}
	return true;
}

bool CXMLConfig::saveBakConfigFile()
{
	if (m_doc == NULL || m_strFilename == "")
	{
		return false;
	}

	//将原xml文档的最后四位去掉加上_bak.xml就是备份文档的名字
	string strBakFilename = m_strFilename.substr(0,m_strFilename.length()-4);
	strBakFilename += "_bak.xml";
	int nRel = xmlSaveFormatFileEnc(strBakFilename.c_str(),m_doc,"UTF-8",1);
	if (nRel == -1)
	{
		return false;
	}
	return true;
}

bool CXMLConfig::loadBakConfigFile()
{
	if (m_doc == NULL || m_strFilename == "")
	{
		return false;
	}
	//将原xml文档的最后四位去掉加上_bak.xml就是备份文档的名字
	string strBakFilename = m_strFilename.substr(0,m_strFilename.length()-4);
	strBakFilename += "_bak.xml";

	if (m_doc != NULL)
	{
		xmlFreeDoc(m_doc);
	}
	m_doc = xmlReadFile(strBakFilename.c_str(),"UTF-8",XML_PARSE_RECOVER);  //解析文件
	return true;
}

string CXMLConfig::bytestohexstring(char* bytes,int bytelength)
{
  string str("");
  string str2("0123456789ABCDEF");
   for (int i=0;i<bytelength;i++) {
     int b;
     b = 0x0f&(bytes[i]>>4);
     str.append(1,str2.at(b));
     b = 0x0f & bytes[i];
     str.append(1,str2.at(b));
   }
   return str;
}