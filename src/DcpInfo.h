/*
*@file:DcpInfo.h
*@brief:DCP�������ߺ�����
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __DCPINFO_H__
#define __DCPINFO_H__

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMError.hpp>
//#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <exception>

#ifndef __FUN_H__
#include "fun.h"
#endif
#ifndef __PKLINFO_H__
#include "PKLInfo.h"
#endif
#ifndef __HASHINFO_H__
#include "HashInfo.h"
#endif

//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
#include <uuid/uuid.h>
//@modify end;

XERCES_CPP_NAMESPACE_USE

namespace Content
{
#ifndef KDM_INFO
#define KDM_INFO
	//KdmInfo
	typedef struct KdmInfo
	{
		std::string path;//�����Ǵ����ļ�·����ftp url
		std::string filename;
		std::string kdmUuid;
		std::string cplUuid;
		std::string playerSn;
		std::string startTime;
		std::string endTime;
		int  status;//success:0,failed:1,nofile:2,error,etc.
		std::string deviceTypeNumber;   //�豸�ͺ�,��X509SubjectName�У���CN=�����
	}KdmInfo;
#endif

	namespace Dcp
	{
		//˵������ʾPkl�Ľṹ�壬�൱��1��dcp��Ӱ����
		//pklinfo
		typedef struct asset_pkl{
			string uuid;
			string annotationtext;
			string hash;
			string type;
			string originalFileName;
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			string Path_NotFullPath;	//��assetmap��ԭʼ·��������������·��
			//@modify end;
			unsigned long long size;
		}asset_pkl_t;
		typedef struct pkl_Info{
			string uuid;
			string annotationText;
			string issueDate;
			string issuer;
			string creator;

			vector<asset_pkl_t> asset;
			unsigned long long int allFileLength;

			//@author zhangmiao@oristartech.com
			//@date [2015-04-17]
			//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
			//@new
			int nFilmPackageFormat;	//��Ӱ����ʽ�ֶΣ�0 = asdcp��1 = smpte
			//@modify end;

		}pkl_Info_t;

		//˵������ʾCpl�Ľṹ��
		//cplinfo
		typedef struct asset_cpl{
			string uuid;
			string annotationText;
			string editRate;
			string intrinsicDuration;
			string entryPoint;
			string duration;
			string keyId;
			string frameRate;
			string screenAspectRatio;
			string language;
			string kind; //mainpicture, mainsound
		}asset_cpl_t;
		typedef struct cpl_Info{
			string uuid;
			string annotationText;
			string issueDate;
			string issuer;
			string creator;
			string contentTitleText;
			string contentKind;
			string path;

			vector<asset_cpl_t> asset;
		}cpl_Info_t;

		//˵������ʾAssetmap�Ľṹ�壬�൱��1��Assetmap��
		//assetmapinfo
		typedef struct asset_assetmap{
			string uuid;//must
			string annotationtext;
			string kind;
			string path;//must
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			string Path_NotFullPath;	//��assetmap��ԭʼ·��������������·��
			//@modify end;
			unsigned long long Length;//must
		}asset_assetmap_t;
		typedef struct assetmap_Info{
			string assetmapUuid;
			vector<asset_assetmap_t> asset;
		}assetmap_Info_t;

		//dcpinfo
		typedef struct dcp_Info{
			string dcpUuid;
			string dcpSource;//the absolute path of dcp storage

			string annotationText; //dcp name
			string issueDate;
			string issuer;
			string creator;

			vector< cpl_Info_t > vCpl;
			//assetmap_Info_t aAsset;
			pkl_Info_t      aPkl;

		}dcp_Info_t;

		typedef struct AMInfo
		{
			vector< dcp_Info_t > vDcp;
			assetmap_Info_t aAssetmap_Info;

			string dcpPath;//dcp����·���������Ǵ��̾���·����ftp url
			string assetmapName;//assetmap�ļ��������ִ�Сд
			long long int assetmapLength;//assetmap�ļ���С

			//@author zhangmiao@oristartech.com
			//@date [2015-04-17]
			//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
			//@new
			int nFilmPackageFormat;	//��Ӱ����ʽ�ֶΣ�0 = asdcp��1 = smpte
			//@modify end;

		}AMInfo_t;

		typedef struct file_Info{
			string f_id;
			string f_path;    //����·��
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//@new
			string f_PathOri;  //assetmap��ԭʼ·��
			//@modify end;
		}file_Info_t;
	};
};

#define ASSETMAP_N_MAX 10240 
#define PKL_N_MAX 64 
#define KDM_N_MAX 512 
#define CPL_N_MAX 512

//���ڽ���xml�����ĵĸ�����
// ---------------------------------------------------------------------------
//  Simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------
class DOMCountErrorHandler : public DOMErrorHandler
{
public:
    //  Constructors and Destructor
    DOMCountErrorHandler();
    ~DOMCountErrorHandler();
    //  Getter methods
    bool getSawErrors() const;
    //  Implementation of the DOM ErrorHandler interface
    bool handleError(const DOMError& domError);
    void resetErrors();
private :
    //  Unimplemented constructors and operators
    DOMCountErrorHandler(const DOMCountErrorHandler&);
    void operator=(const DOMCountErrorHandler&);
    bool    fSawErrors;
};
inline bool DOMCountErrorHandler::getSawErrors() const
{
        return fSawErrors;
}

//���ڽ���DCP��������
class DcpInfo
{
public:
    DcpInfo();
    ~DcpInfo();

	//----zhangmiao:begin--20120605---
	/************************************************* 
	*@brief��  ����·��������������DCP�������Ϣ
	*@param��  srcPath �� ����DCP�ı���·��  :[in]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int CreateDCPInfo(string srcPath , string& error );

	/************************************************* 
	*@brief��  ����Assetmap�ļ�����������ֵ�AMInfo�ṹ����
	*@param��  xmlFile �� Assetmap�ļ��ı���·����  :[in]
	*@param��  aAMInfo �� ������������ֵ�AMInfo�ṹ��  :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int Parse_Assetmap_New( std::string xmlFile , Content::Dcp::AMInfo& aAMInfo , string& error );

	/************************************************* 
	*@brief��  ����PKL�ļ�����������ֵ�dcp_Info_t�ṹ����
	*@param��  pkl �� PKLInfo�Ǳ�ʾpkl�ļ��Ľṹ����       :[in]
	*@param��  dcp �� ������������ֵ�dcp_Info_t�ṹ��     :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int Parse_Pkl_New( PKLInfo *pkl , Content::Dcp::dcp_Info_t& dcp , string& error );

	/************************************************* 
	*@brief��  ����CPL�ļ�����������ֵ�cpl_Info_t�ṹ����
	*@param��  cplPath �� CPL�ļ��ı���·����             :[in]
	*@param��  cpl �� ������������ֵ�cpl_Info_t�ṹ����  :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int Parse_Cpl_New(const char *cplPath, Content::Dcp::cpl_Info_t &cpl , string& error );

	/************************************************* 
	*@brief��  �ؽ�Assetmap�ļ�����������ֵ�KdmInfo�ṹ����
	*@param��  xmlFile �� Assetmap�ļ��ı���·����                         :[in]
    *@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���                  :[in]
	*@param��  aPkl ��  �������Pkl�ļ��Ľṹ���н���ɸѡ�ؽ�Assetmap�ļ�  :[in]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int ReBuild_Assetmap(  std::string xmlFile ,const string &pklUuid, Content::Dcp::pkl_Info_t& aPkl , string& error );
	
	/************************************************* 
	*@brief��  ����KDM�ļ�����������ֵ�KdmInfo�ṹ����
	*@param��  sFile �� KDM�ļ��ı���·����                :[in]
	*@param��  kdmInfo �� ������������ֵ�KdmInfo�ṹ����  :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int ParseKdm_New( std::string sFile , Content::KdmInfo& kdmInfo, string& error );
    /************************************************* 
	*@brief��  ����KDM�ļ�����������ֵ�KdmInfo�ṹ����
	*@param��  path    �� KDM�ļ���׷��Ŀ¼            :[in]
	*@param��  kdmList     ������KDM�б�		       :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int GetKdmInfo(  std::string &path, std::vector<Content::KdmInfo> &KdmList, string& error);
    /************************************************* 
	*@brief��  ����KDM�ļ�����������ֵ�KdmInfo�ṹ����
	*@param��  KdmFileName �� KDM�ļ��ı���·����                :[in]
	*@param��  kdmInfo �� ������������ֵ�KdmInfo�ṹ����        :[out]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
	int GetKdmInfo( const std::string &KdmFileName, Content::KdmInfo& kdmInfo, string& error );
	//---zhangmiao:end---20120605----

	//----zhangmiao---begin---20120615----
	//AMInfo�б�ÿ��Ԫ�ر�ʾ1��AMInfo����
	vector< Content::Dcp::AMInfo > vAMInfo_t;
	//assetmap_Info�б�1��assetmap_Info�����ʾassetmap�ļ��еľ�������
	std::vector< Content::Dcp::assetmap_Info > assetmap_InfoVt;
	//pkl_Info_t�б�1��pkl_Info_t�����ʾPKL�ļ��еľ�������
	std::vector< Content::Dcp::pkl_Info_t > pkl_Info_Vt;
	//����pkl Listָ��
	//PKLInfo* getPKLInfo(){ return (PKLInfo*)pklList;}
	//----zhangmiao:end--20120615------

	//@author zhangmiao@oristartech.com
	//@date [2014-04-04]
	//@brief ��Ҫ����д������־�Ĵ���
	//@new
//#define WRITE_DCP_ELOG 
	#define WDELOG(str) WriteErrorLog(str);
	int WriteErrorLog( const std::string& strError );
	//@modify end;

private:
	/************************************************* 
	*@brief��  DcpInfo��ʼ����
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
    int init();
    /************************************************* 
	*@brief��  XML�ĵ���ģʽ��Schema����֤���г�ʼ����
	*@param��  f_schechk �� ��ʾ�Ƿ����ģʽ��Schema����֤      :[in]
    *@param��  schePath �� xmlģʽ��Schema����֤�ļ��ı���·��  :[in]
	*@Return�� ������,0=�ɹ�����0������.
	*************************************************/ 
    int ParseInit(int f_schechk,const std::vector<std::string> &schePath);
	/************************************************* 
	*@brief��  Parse XML�ĵ���
	*@param��  xmlPath �� xml�ļ��ı���·��  :[in]
	*@Return�� �����ɹ�����DOMDocument�����ָ�룬����NULL=����
	*************************************************/ 
    DOMDocument *ParseXML(const std::string &xmlPath);
	/************************************************* 
	*@brief��  ����ASSETMAP�ļ�
	*@param��  path �������������ļ�·����������"/"��\����β�� :[in]       
	*@param��  assetmapList�� �������������ļ���·������1��2ά����         ��out��
	*@param��  max�� ������������ļ���         ��in��
	*@param��  mapN �� �������������ļ�������   :[out]
	*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
	*************************************************/
    int getAssetmapList(const char *path,char **assetmapList,int *mapN,int max , string& error );//search ASSETMAP in the path; 
	/************************************************* 
	*@brief��  ����KDM�ļ�
	*@param��  path �������������ļ�·����������"/"��\����β�� :[in]         
	*@param��  kdmList�� �������������ļ���·������1��2ά����         ��out��
	*@param��  max�� ������������ļ���         ��in��
	*@param��  kdmN �� �������������ļ�������   :[out]
	*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
	*************************************************/
    int getKdmList(const char *path,char **kdmList, int *kdmN, int max , string& error);//search KDM in the path ;
  
	/************************************************* 
	*@brief��  ����xml�ĵ��ڵ�Node������Path·����
	*@param��  path ������������xml�ļ����·������һ��·���б�   :[in]
	*@param��  root�� ����root����㣬��������                   ����in��
	*@Return�� �ɹ������ҵ���DOMNode�����ָ�룬����NULL=����û���ҵ�.
	*************************************************/
    DOMNode *getNodeByXpath(DOMNode *root,std::vector<std::string> path);


	//@author zhangmiao@oristartech.com
	//@date [2013-09-22]
	//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
	//@new
	//��֤�Ƿ���cpl�ļ�
	//IsCplFile��0 = ����cpl�ļ���1 = ��cpl�ļ���
	int VerifyIsCplFile4Pkl_Asset(const char* cPkl_AssetPath, int& IsCplFile , string& error);
    //@modify end;

private:
	//�����õ���Assetmap�ļ���Ϣ�б�
    std::vector<Content::Dcp::file_Info_t> assetmapFile;

	//������õ���pkl�ļ��б�
    PKLInfo *pklList[PKL_N_MAX];
	//pkl�ļ��ĸ���
    int  pklN;
	//assetmap Id �б�
    std::vector<std::string> assetmapIdVt;
  
	//xml����������ָ��
    DOMImplementation *impl;
    DOMLSSerializer   *serializer;
    DOMLSOutput       *output;//XMLFormatTarget   *formatTarget;
    DOMLSParser       *parser;
    DOMDocument       *docRet;
    DOMCountErrorHandler errorHandler;
	//dcp���Ĵ�С
    unsigned long long dcpSize;

public:
	
};

//���ڽ���xml�����ĵĸ�����
class StrX
{
public :
    //  Constructors and Destructor
    StrX(const XMLCh* const toTranscode)
    {   // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
        //std::cout << "constractor:" << fLocalForm << std::endl;
    }
    ~StrX()
    {
        //std::cout << "deconstractor:" << fLocalForm << std::endl;
        XMLString::release(&fLocalForm);
    }
    //  Getter methods
    const char* localForm() const
    {
        return fLocalForm;
    }
private :
    char*   fLocalForm;
};
inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}

class XStr
{
public :
    //  Constructors and Destructor
    XStr(const char* const toTranscode)
    {
        fUnicodeForm = XMLString::transcode(toTranscode);
    }
    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }
private :
    XMLCh* fUnicodeForm;
};
#define X(str) XStr(str).unicodeForm()


#endif

