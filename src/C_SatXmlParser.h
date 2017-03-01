//@file:C_SatXmlParser.h
//@brief: �������xml��ʽ������
//@author: zhangmiao@oristartech.com
//date:  2013-05-28
#ifndef _SAT_XML_PARSER_H
#define _SAT_XML_PARSER_H
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#ifndef _SAT_DCP_H_
#include "C_Satellite.h"
#endif

//#define _DEBUG_

#define RESPONSE_STATUS_OK						0
#define XML_PARSER_ERROR						-201		//XML��Ϣ��������
#define RESPONSE_STATUS_ERROR					-202		//���"status"���Բ�����"OK"�����ʾ״̬����
#define RESPONSE_FORMAT_ERROR					-203		//XML��Ϣ��Ӧ��ʽ����
#define XML_NODE_NOEXIST_ERROR					-204		//ĳ���ڵ㲻����
#define XML_CHILDNODES_NOEXIST_ERROR			-205		//ĳ���ӽڵ㼯������

using namespace std;

XERCES_CPP_NAMESPACE_USE

class C_SatXmlParser
{
public:
	C_SatXmlParser(const char*str);

	~C_SatXmlParser();
	
	//************************************
	// Method:    GetDcpInfoList_parser
	// FullName:  C_SatXmlParser::GetDcpInfoList_parser
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: std::vector< Satellite::DCPInfo > & DcpList
	//************************************
	int GetDcpInfoList_parser( std::vector< Satellite::DCPInfo >& DcpList );

	//************************************
	// Method:    GetFilmFtpInfo_parser
	// FullName:  C_SatXmlParser::GetFilmFtpInfo_parser
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: Satellite::FTP_DCPInfo & rFTP_DCPInfo
	//************************************
	int GetFilmFtpInfo_parser( Satellite::FTP_DCPInfo& rFTP_DCPInfo );

protected:

private:
	xercesc::XercesDOMParser *m_xml_parser;
	xercesc::InputSource* m_inputsource;
	xercesc::DOMDocument* m_xmlDoc;
	xercesc::DOMElement* m_elementRoot;
};

#endif

//���ڽ���xml�����ĵĸ�����
class XMLCh2Char
{
public :
	//  Constructors and Destructor
	XMLCh2Char()
	{
		fLocalForm = NULL;
	}
	XMLCh2Char(const XMLCh* toTranscode)
	{   // Call the private transcoding method
		fLocalForm = XMLString::transcode(toTranscode);
		//std::cout << "constractor:" << fLocalForm << std::endl;
	}
	~XMLCh2Char()
	{
		if (fLocalForm)
		{
			XMLString::release(&fLocalForm);
			fLocalForm = NULL;
		}
	}
	void operator =(XMLCh* xmlch)
	{
		fLocalForm = XMLString::transcode(xmlch);
	}

	//  Getter methods
	const char* localForm() const
	{
		return fLocalForm;
	}

	char* get_char_str()
	{
		return fLocalForm;
	}

private :
	char*   fLocalForm;
};
inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const XMLCh2Char& toDump)
{
	target << toDump.localForm();
	return target;
}

class Char2XMLCh
{
public :
	//  Constructors and Destructor
	Char2XMLCh()
	{
		fUnicodeForm = NULL;
	}
	Char2XMLCh(const char* toTranscode)
	{
		fUnicodeForm = XMLString::transcode(toTranscode);
	}
	~Char2XMLCh()
	{
		if (fUnicodeForm)
		{
			XMLString::release(&fUnicodeForm);
			fUnicodeForm = NULL;
		}
	}

	void operator = (const char* chstr)
	{
		fUnicodeForm = XMLString::transcode(chstr);
	}

	const XMLCh* unicodeForm() const
	{
		return fUnicodeForm;
	}

	XMLCh* get_xml_str()
	{
		return fUnicodeForm;
	}

private :
	XMLCh* fUnicodeForm;
};
#define C2X(str) Char2XMLCh(str).unicodeForm()

