/*
*@file:content.h
*@brief:����dcp�� ���غ�ftp
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*@History:
*@ zhangmiao@oristartech.com  2012-08-30  �޸�CplInfo�ṹ�壬����CPL�ֶ�status��allFileLength��
*/
#ifndef __CONTENT_H__
#define __CONTENT_H__
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#ifndef __DCPINFO_H__
#include "DcpInfo.h"
#endif

//VideoType��ֵ:1����Mpeg2��0����2D��ʽ��Jpeg2000
typedef enum VIDEO_TYPE
{
	VIDEO_TYPE_UNKNOWN=-1,VIDEO_TYPE_JPEG2000=0,VIDEO_TYPE_MPEG2
}VIDEO_TYPE;

namespace Content
{
	typedef struct OtherFileInfo
	{
		//@author zhangmiao@oristartech.com
		//@date [2013-07-18]
		//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
		//@new
		std::string Uuid;
		std::string originalfileName;
		//@modify end;
		std::string fileName;
		long long int fileLength;
	}OtherFileInfo;

	//˵������ʾMxf�Ľṹ��
	typedef struct Mxf_FileInfo
	{
		std::string mxfUuid;
		std::string fileName;
		long long int length;
	}/*MxfInfo*/Mxf_FileInfo;

	//˵������ʾCpl�Ľṹ��
	typedef struct CplInfo
	{
		std::string cplUuid;
		std::string cplName;
		std::string cplTitle;
		std::string cplIssueDate;
		std::string cplssuer;
		std::string cplCreator;			
		long long int cplLength;   //cpl�ļ��Ĵ�С
		std::vector<std::string> mainPicList;//��¼UUID
		std::vector<std::string> mainSoundList;//��¼UUID
		int type;//ӰƬ���ͣ���桢��Ƭ����ƪ��10������
		int is3D;
		int rate;//����
		int duration;              //����ʱ�䣬��������Ϊ��λ��int����
		double Duration_double;    //����ʱ�䣬��������Ϊ��λ��double����
		float ScreenAspectRatio;//��߱�
		int needKdm;     //�Ƿ���ܣ�1��Ҫ��0����Ҫ

		//@author zhangmiao@oristartech.com
		//@date 2012-08-30
		//@brief �޸�CplInfo�ṹ�壬����CPL�ֶ�status��allFileLength��
		//@new
		long long int allFileLength;    //CPL����С���ļ������ܺϣ�
		int status;      //״̬���Ƿ�������0������1 DCP������
		//@modify end;

		//@author zhangmiao@oristartech.com
		//@date 2013-12-09
		//@brief ��Ҫ�޸�CplInfo�ṹ�壬����CPL�ֶ�MainSoundLanguage�Ĵ������
		//@new
		std::string MainSoundLanguage;       //��ʾӰƬ���ԣ��������Դ��룬���cpl�ļ���û�и��ֶΣ���Ĭ�Ϸ���δ֪,"unknown"��
		//@modify end;

		//@author zhangmiao@oristartech.com
		//@date 2015-05-04
		//@brief ���Ӵ��룬�ж���Ƶ����
		//@new
		//nVideoType��ֵ:1����Mpeg2��0����2D��ʽ��Jpeg2000
		VIDEO_TYPE nVideoType;
		//@modify [2015-05-04] end;

	}CplInfo;

	//˵������ʾPkl�Ľṹ�壬�൱��1��dcp��Ӱ����
	typedef struct PklInfo
	{
		std::string pklUuid;
		std::string pklName;
		std::string AnnotationText;
		std::string IssueDate;
		std::string	Issuer;
		std::string	Creator;
		long long int pklLength;//pkl�ļ��Ĵ�С
		long long int allFileLength;//pkl�а����������ļ���С���ܺ�
		std::vector<CplInfo> cplList;  //cpl �б�
		std::vector</*MxfInfo*/Mxf_FileInfo> mxfList;  //mxf �б�
		//@author zhangmiao@oristartech.com
		//@date 2013-07-18
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		std::vector<OtherFileInfo> fileInfoList;    //��������ļ��б����磺��Ļ����ļ�
		//@modify end;

		//@author zhangmiao@oristartech.com
		//@date [2015-04-17]
		//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
		//@new
		int nFilmPackageFormat;	//��Ӱ����ʽ�ֶΣ�0 = asdcp��1 = smpte
		//@modify end;

	}PklInfo;
	//˵������ʾAssetmap�Ľṹ�壬�൱��1��Assetmap��
	typedef struct AssetmapInfo
	{
		std::string assetmapUuid;
		std::string assetmapName;//assetmap�ļ��������ִ�Сд
		long long int assetmapLength;//assetmap�ļ���С
		long long int dcpLength;//����dcp�����ļ���С���ܺ�
		std::string dcpPath;//dcp����·���������Ǵ��̾���·����ftp url
		std::vector<PklInfo> pklList;
		std::vector<OtherFileInfo> fileInfo;
	}AssetmapInfo;
	//˵��������ָ��DCP��У����ɰٷֱȡ�
	typedef struct HashInfo_t
	{
		std::string path;
		int  percent;
		int  status;          //checking,notstart,pausing,finished,error,etc.
		std::string filename; //���У���쳣���򷵻��쳣�ļ�����������ֶ���Ч
	}Hashinfo;

	//File Copy ��ص����ݽṹ
	typedef struct FileCopyInfo      //�������ڶϵ�����
	{
		int bIsFinished;
		std::string filename;
		int fileSize;           //�ļ���С����λMB
		int transferredSize;    //�Ѵ����С����λMB
		FileCopyInfo()
		{
			bIsFinished=0;
			fileSize=0;
			transferredSize=0;
			filename="";
		}
	}FileCopyInfo;

	typedef struct CopyInfo
	{
		std::string source;
		std::string dest;
		int percent;
		int status;          //coping, notstart, pausing,finished,error.etc
		std::vector<FileCopyInfo> copyInfo;   //����ÿ���ļ��Ŀ���״��
	}CopyInfo;
};
//CPL��������
typedef enum CPL_CONTENT_KIND 
{
	feature=1, //��Ƭ
	trailer,   //��ӰԤ��Ƭ;Ԥ���Ŀ��2-3�֣�
	test,      //����Ƭ
	teaser,    ////�̵�ӰԤ��Ƭ;Ԥ���Ŀ��<1�֣�
	rating,    // �������ʾ
	advertisement,  //���
	shorts,    // ��Ƭ
	transitional,  //���ɣ�����
	psa,     // �����Ƭ
	policy    // ����
}CPL_CONTENT_KIND;
//CPL�������ͣ�mapӳ���
extern std::map<std::string,CPL_CONTENT_KIND> mapCPL_ContentKind;

//hash check״̬
typedef enum HASH_CHECK_STATUS_KIND 
{
	STATUS_ERROR=-1,STATUS_CHECKING=1,STATUS_NOSTART,STATUS_PAUSEING,STATUS_FINISHED,
}HASH_CHECK_STATUS;
//Kdminfo copy״̬
typedef enum KDMINFO_STATUS_KIND 
{
	KDMINFO_STATUS_ERROR=-1,KDMINFO_STATUS_SUCESSED,
}KDMINFO_STATUS_KIND;

//dcp copy״̬
typedef enum DCP_COPY_STATUS_KIND 
{
	DCP_STATUS_ERROR=-1,DCP_STATUS_COPING=1,DCP_STATUS_NOSTART,DCP_STATUS_PAUSEING,DCP_STATUS_FINISHED,
}DCP_COPY_STATUS_KIND;



//@author zhangmiao@oristartech.com
//@date [2013-08-12]
//@brief ��Ҫ������Satelliteר�����ǽӿڵĴ������
//@new
/************************************************* 
*@brief��  ��IP��ַ��ָ��DCPר�����ǽ����豸�У�����FTP������ȫ����DCP����
*@param��  Ip �� ����dcp������ר�����ǽ����豸��IP��ַ     :[in]
*@param��  dcpList     �����ص�dcp�����б�		           :[out]
*@param��  error     �����ش�����Ϣ		                   :[out]
*@Return�� ������,0 = �ɹ�����0������.
*************************************************/
int SearchSatDcpList( const std::string &Ip , std::vector<Content::AssetmapInfo> &dcpList , std::string& userName , std::string& passWd ,
					   std::string& error , unsigned short usPort = 20080 );

/************************************************* 
*@brief��  ��IP��ַ��ָ��DCPר�����ǽ����豸�����Ͷ�ӦӰƬ�����������
*@param��  Ip �� ����dcp������ר�����ǽ����豸��IP��ַ     :[in]
*@param��  PklUuid �� ֻ���ָ��id��PKL�ļ������в���      :[in]
*@param��  Status �����status = 1����ʾӰƬ������ϡ�status = 0����ʾ����ʧ��  :[in]
*@param��  error     �����ش�����Ϣ		                   :[out]
*@Return�� ������,0 = �ɹ�����0������.
*************************************************/
//��ӦӰƬ������ϣ����status = 1����ʾӰƬ������ϡ�status = 0����ʾ����ʧ�ܡ�
int Sate_SetFTPDownLoadFinished( const std::string& Ip , const std::string& PklUuid , const int Status , std::string& error );
//@modify end;

/************************************************* 
*@brief��  ��path��ָ�ı���·���У�����ȫ����DCP����
*@param��  path �� ����dcp�����ı���·����������"/"��β  :[in]
*@param��  dcpList     �����ص�dcp�����б�		       :[out]
*@Return�� ������,0=�ɹ�����0������.
*************************************************/  
//��������
int SearchDcpList( const std::string &path, std::vector<Content::AssetmapInfo> &dcpList ,std::string& error );
/************************************************* 
*@brief��  ��ftpUrl��ָ��·���У�����FTP������ȫ����DCP����
*@param��  ftpUrl �� ����dcp������FTP������URL��������"/"��β  :[in]
*@param��  dcpList     �����ص�dcp�����б�		               :[out]
*@Return�� ������,0 = �ɹ�����0������.
*************************************************/
//����FTP������
//ftpUrl���ַ���������ftp://userName:passwd@172.23.140.50:21/dir1/dir2/
int SearchFtpDcpList(const std::string &ftpUrl, std::vector<Content::AssetmapInfo> &dcpList ,std::string& error );
/************************************************* 
*@brief��  �ú����������DCP������������У�顣
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//DcpУ��
int HashDcp( const std::string &path, const std::string &pklUuid ,std::string& error );
/************************************************* 
*@brief��  �ú�����ȡhashУ�����
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@param��  hash  �� ����ָ��DCP��У����ɰٷֱȡ�      :[out]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//��ȡhashУ�����
int GetHashPercent( const std::string &path, const std::string &pklUuid, Content::Hashinfo &hash ,std::string& error );
/************************************************* 
*@brief��  �ú���ȡ��hashУ��
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//ȡ��hashУ��
int CancelHash( const std::string &path, const std::string &pklUuid ,std::string& error );
/************************************************* 
*@brief��  �ú���ɾ��DCP
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//ɾ��DCP
int DeleteDcp( const std::string &path, const std::string &pklUuid , std::string& error );
/************************************************* 
*@brief��  �ú���DCP���룬֧��2��ģʽ��1�δ����ϵ�������
*@param��  source �� source����ΪDCP����ֱ��·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���            :[in]
*@param��  dest ��������DCPֱ�ӿ�����destĿ¼�£����û��destĿ¼��������·����������"/"��β  :[in]
*@param��  copyInfo �� ��һ�δ��䣬copyInfo����Ϊ�գ��ϵ�����ʱ������ָʾÿ���ļ������״����:[in|out]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//DCP���롣���һ��ASSETMAP�ļ��У��������PKL�ļ�����Ϣ����ֻ����pklUuid��ָ����ӰƬ����ͬʱ��ΪӰƬ������ASSETMAP�ļ���
int CopyDcp( const std::string &source, const std::string &pklUuid, const std::string &dest,
			 std::vector<Content::FileCopyInfo> &copyInfo , std::string& error );
//Copy PKL AssetList File
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int CopyDcpSub1( const std::string &source, const std::string &Dest, Content::Dcp::pkl_Info_t& aPkl, std::string& error );
int CopyDcpSub1( const std::string &source, const std::string &Dest, Content::Dcp::pkl_Info_t& aPkl, std::string& error );
//@modify end;
//Copy PKL File
int CopyDcpSub2( const std::string &pklName, const std::string &Dest, std::string& error );
//Copy AssetMap File ���������Ҫ�ؽ�AssetMap�ļ�
int CopyDcpSub3( const std::string &AssetmapName, const std::string &pklUuid, const std::string &Dest,
				 DcpInfo& dcp, Content::Dcp::pkl_Info_t& aPkl , std::string& error );
/************************************************* 
*@brief��  �ú���ȡ��DCP�������
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@param��  info �� ����ָ��DCP�����������ɰٷֱȡ�   :[out]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//ȡ��DCP�������
int GetCopyPercent(const std::string &source , const std::string &pklUuid, Content::CopyInfo &info, std::string& error );
/************************************************* 
*@brief��  �ú���ȡ��DCP����
*@param��  path �� ����DCP��������·����������"/"��β  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//ȡ��DCP����
int CancelCopy( const std::string &source, const std::string &pklUuid , std::string& error );
/************************************************* 
*@brief��  ��path��ָ�ı���·���У�����ȫ����KDM�б�
*@param��  path �� ·�������԰����ļ������������ļ�����׷��Ŀ¼�������ļ�����ֱȡ�ø��ļ���Ϣ��·����������"/"��β  :[in]
*@param��  kdmList     ������KDM�б�		       :[out]
*@Return�� ������,0=�ɹ�����0������.
*************************************************/  
//KDM�б�
int GetKdmInfo( const std::string &path, std::vector<Content::KdmInfo> &kdmList, std::string& error );
/************************************************* 
*@brief��  ��path��ָ�ı���·���У�KDMɾ����
*@param��  path �� ·�������԰����ļ������������ļ�����׷��Ŀ¼�������ļ�����ֱȡ�ø��ļ���Ϣ��·����������"/"��β  :[in]
*@Return�� ������,0=�ɹ�����0������.
*************************************************/ 
//KDMɾ��
int DeleteKdm( const std::string &path , std::string& error );
/************************************************* 
*@brief��  KDM���롣
*@param��  kdmList ��ͨ������GetKdmInfo()�󣬷��ص�KDM�б� :[in]
*@param��  dest �� ���غ��Ŀ��·����·����������"/"��β   :[in]
*@Return�� ������,0=�ɹ�����0������.
*************************************************/ 
//KDM���룬dest��������ֱ�ӽ�����
int ImportKdm( std::vector<Content::KdmInfo> &kdmList, const std::string &dest , std::string& error );
/************************************************* 
*@brief��  ��FTP����������KDM��Ŀǰֻʵ����ֱ������KDM�Ĺ��ܡ�
*@param��  kdmList ��ͨ��KdmInfo�е�path��filenameָ��ѽ���ص��ļ�  :[in]
*@param��  dest �� ���غ��Ŀ��·����·����������"/"��β            :[in]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//��FTP����������KDM
int DownLoadKdm( std::vector<Content::KdmInfo> &kdmList, const std::string &dest , std::string& error );

/************************************************* 
*@brief��  ��ͨ�ļ�������
*@param��  src ��Դ�ļ���            :[in]
*@param��  des ��Ŀ���ļ���          :[in]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//��ͨ�ļ�����
int cp_file( const char *src, const char *des );
/************************************************* 
*@brief��  �ļ��������ϵ�������֧�ֻ�ȡ���ȡ�
*@param��  src ��Դ�ļ���                                 :[in]
*@param��  des ��Ŀ���ļ���                               :[in]
*@param��  from ����from��ָ��λ�ÿ�ʼ����                :[in]
*@param��  rlTransferredSize �������Ѿ���������ֽ���     :[out]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//�ļ��������ϵ�������֧�ֻ�ȡ����
int cp_file_BP( const char *src, const char *des , unsigned long long& rlTransferredSize ,unsigned long long from = 0 );
/************************************************* 
*@brief��  ��ͨ�ļ�������֧�ֻ�ȡ����
*@param��  src ��Դ�ļ���                                 :[in]
*@param��  des ��Ŀ���ļ���                               :[in]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//�ļ�������֧�ֻ�ȡ����
int cp_file_Mode2( const char *src, const char *des );
/************************************************* 
*@brief��  �ϵ�������ʽ������DCP����
*@param��  source �� source����ΪDCP����ֱ��·����������"/"��β     :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���               :[in]
*@param��  dest ��������DCPֱ�ӿ�����destĿ¼�£����û��destĿ¼��������·����������"/"��β  :[in]
*@param��  copyInfo �� ��һ�δ��䣬copyInfo����Ϊ�գ��ϵ�����ʱ������ָʾÿ���ļ������״����:[in|out]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//�ϵ�������ʽ������DCP����
int CopyDcp_BPTrans( const std::string &source, const std::string &pklUuid, const std::string &dest,
					 std::vector<Content::FileCopyInfo> &copyInfo , std::string& error );
//�ϵ�������ʽ������copyInfo�е�ÿ���ļ�������CPL�ļ�ֱ�ӿ���
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int CopyDcp_BPTransSub1( const std::string &Dest, Content::Dcp::pkl_Info_t& aPkl, std::vector<Content::FileCopyInfo> &copyInfo , std::string& error );
int CopyDcp_BPTransSub1( const std::string &source, const std::string &Dest, Content::Dcp::pkl_Info_t& aPkl, std::vector<Content::FileCopyInfo> &copyInfo , std::string& error );
//@modify end;
/************************************************* 
*@brief��  FTP����DCP
*@param��  ftpUrl ������dcp������FTP������URL��������"/"��β��     :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���                           :[in]
*@param��  dest �� ������DCPֱ�ӿ�����destĿ¼�£����û��destĿ¼��������·����������"/"��β  :[in]
*@param��  copyInfo �� ��һ�δ��䣬copyInfo����Ϊ�գ��ϵ�����ʱ������ָʾÿ���ļ������״����:[in|out]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//FTP����DCP
int DownLoadDcp(const std::string &ftpUrl, const std::string &pklUuid, const std::string &dest, std::vector<Content::FileCopyInfo> &copyInfo, std::string& error );
/************************************************* 
*@brief��  ȡ��FTP����DCP
*@param��  ftpUrl ������dcp������FTP������URL��������"/"��β��     :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���              :[in]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/ 
//ȡ��FTP����DCP
int CancelDownLoad( const std::string &ftpUrl, const std::string &pklUuid, std::string& error );

/************************************************* 
*@brief��  ȡ��FTP����DCP����
*@param��  ftpUrl ������dcp������FTP������URL��������"/"��β�� :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���  :[in]
*@param��  info �� ����ָ��DCP�����������ɰٷֱȡ�   :[out]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//ȡ��FTP����DCP����
int GetDownLoadPercent(const std::string &ftpUrl, const std::string &pklUuid, Content::CopyInfo &info, std::string& error );
/*************************************************
*@brief��  �ϵ�������ʽ������DCP����
*@param��  ftpUrl ������dcp������FTP������URL��������"/"��β��                  :[in]
*@param��  pklUuid �� ֻ���ָ��id��PKL�ļ������в���                           :[in]
*@param��  dest ��������DCPֱ�ӿ�����destĿ¼�£����û��destĿ¼��������·����������"/"��β  :[in]
*@param��  copyInfo �� ��һ�δ��䣬copyInfo����Ϊ�գ��ϵ�����ʱ������ָʾÿ���ļ������״����:[in|out]
*@Return�� ������,0=�ɹ�����0=����
*************************************************/
//�ϵ�������ʽ������DCP����
int DownLoadDcp_BPTrans( const std::string &ftpUrl, const std::string &pklUuid, const std::string &dest,
				         std::vector<Content::FileCopyInfo> &copyInfo , std::string& error );
//����copyInfo�е�ÿ���ļ�
int DownLoadDcp_BPTransSub1( const std::string &Dest, Content::PklInfo& aPkl, std::vector<Content::FileCopyInfo> &copyInfo, std::string& error );
//Download PKL File
int DownLoadDcp_BPTransSub2( const std::string &pklName, const std::string &Dest, std::string& error );
//Download AssetMap File
int DownLoadDcp_BPTransSub3( const std::string &AssetmapName, const std::string &pklUuid, const std::string &Dest,
				              Content::PklInfo& aPkl, std::string& error );

#define CODE_DCPCOPYCANCEL  -90001
#define ONEMB  1048576

#ifndef WIN32
#define O_BINARY       0x8000  
#ifndef strcmpi
#define strcmpi strcasecmp
#endif
#ifndef stricmp
#define stricmp strcasecmp
#endif
#endif

//��ʼ�� mapCPL_ContentKind ӳ���
void init_maplist();

//��ȡPKL���ʲ��б�AssetList�У�ָ���ļ�FileName�Ĵ�С��FileName���ļ���������·����
//����
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int GetFileSize_PKLAssetList( const std::string FileName ,const Content::Dcp::pkl_Info_t& aPkl, long long& Length );
int GetFileSize_PKLAssetList( const std::string FileName ,const Content::Dcp::pkl_Info_t& aPkl, long long& Length , std::string& FullPathFileName );
//@modify end;

//ftp DownLoad
//@author zhangmiao@oristartech.com
//@date [2013-07-18]
//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
//@new
//int GetFileSize_PKLAssetList( const std::string FileName ,const Content::PklInfo& aPkl, long long& Length );
int GetFileSize_PKLAssetList( const std::string FileName ,const Content::PklInfo& aPkl, long long& Length ,std::string& originalfileName );
//@modify end;

//************************************
// Method:    CheckCPLIsValid
// FullName:  CheckCPLIsValid
// Access:    public 
// Returns:   int
// Qualifier:���cpl�ļ���Ч״̬���ڱ��ء�
// Parameter: const std::vector< asset_cpl_t > & vAsset_Cpl
// Parameter: const std::vector< asset_pkl_t > & vAsset_pkl
//************************************
int CheckCPLIsValid( const std::vector< Content::Dcp::asset_cpl_t >& vAsset_Cpl , const std::vector< Content::Dcp::asset_pkl_t >& vAsset_pkl ,std::string& error );

int CheckCPLIsValid4Ftp( const std::string dir ,const std::string assetmapUuid , const cftp::FtpClient& ftp ,
	                     std::vector<Content::AssetmapInfo>& dcpList );

//@author zhangmiao@oristartech.com
//@date 2015-05-05
//@brief ���Ӵ��룬�ж���Ƶ���ͣ�nVideoType��ֵ:1����Mpeg2��0����2D��ʽ��Jpeg2000
int CheckVideoType( VIDEO_TYPE &rVideoType , const std::string &PicUuid , const std::vector< Content::Dcp::asset_pkl_t >& vAsset_pkl );
//@modify [2015-05-05] end;

//��Ԫ�����������ڶ�vector<AssetmapInfo> & dcpList����������Ĭ���á�<���������Ϊ����
struct FuncCompDcpList
{
	// functor for operator<
	bool operator()(const Content::AssetmapInfo& _Left, const Content::AssetmapInfo& _Right) const
	{
		// apply operator< to operands
		//return (_Left.assetmapName < _Right.assetmapName);
		return (_Left.dcpPath < _Right.dcpPath);
	}
};

//error code list
//���������֣����ݹ���ʹ��1001���ϵ�ֵ��
const int nCONTENT_ERROR_Code_Base_Value = 1000;
const int ERROR_CONTENT_FTP_TCPTRANSPORTFAILED		 = -1;
const int ERROR_CONTENT_FTP_FILECANNOTOPEN			 = -2;
const int ERROR_CONTENT_FTP_PARAMETERERROR			 = -3;
const int ERROR_CONTENT_FTP_SERVICEERROR			 = -10;
const int ERROR_CONTENT_FTP_UNKNOWERROR				 = -100;
const int ERROR_CONTENT_FTP_CANCEL_TRANSFILE_ERROR	 = -20;			//FtpClient::CancelTransFile() Error!

const int ERROR_CONTENT_KDM_GETKDMINFOERROR							 = -101;	//GetKdmInfo error
const int ERROR_CONTENT_KDM_DELETEKDM_ERROR							 = -102;	//"DeleteKdm() error!:bad Path"
const int ERROR_CONTENT_KDM_KDMINFOCOUNTISNULL_ERROR				 = -103;	//error:KdmInfoCount is NULL !
const int ERROR_CONTENT_DCP_DONOT_FOUND_PKL						     = -120;	//Do not found pkl
const int ERROR_CONTENT_DCP_PARAM_PATH_IS_NULL						 = -121;	//"Param Path is Null!"
const int ERROR_CONTENT_DCP_UNABLE_TO_DELETE_DIR					 = -122;	//Unable to delete directory
const int ERROR_CONTENT_DCP_PKL_UUID_ERROR							 = -123;	//PklUuid is error
const int ERROR_CONTENT_DCP_CREATE_DCPINFO_ERROR    				 = -124;	//error:CreateDCPInfo()
const int ERROR_CONTENT_DCP_COPY_NO_START							 = -125;	//error:DCP Copy is NO START.
const int ERROR_CONTENT_DCP_PARAM_COPYINFO_ISNULL					 = -126;	//Param copyInfo is NULL!
const int ERROR_CONTENT_DCP_COPYDOWNLOAD_NO_START					 = -127;	//error:DCP DownLoad is NO START.
const int ERROR_CONTENT_DCP_PARSE_ASSETMAP_ERROR					 = -128;	//parseAssetmap error!
const int ERROR_CONTENT_DCP_MAKE_DIR_FAIL 							 = -129;	//Error:Make Directory fail.
const int ERROR_CONTENT_DCP_FILM_FILE_PATH_ISNULL 					 = -130;	//Error:FilmFilePath is NULL!
const int ERROR_CONTENT_DCP_SEARCH_FTP_DCP_LIST 					 = -131;	//error: SearchFtpDcpList
const int ERROR_CONTENT_DCP_SEARCH_DCP_LIST 						 = -132;	//error: SearchDcpList
const int ERROR_CONTENT_DCP_UNABLE_TO_DELETE_ASSETMAP_DIR 			 = -133;	//Unable to delete ASSSETMAP directory
const int ERROR_CONTENT_DCP_CHECKCPL_INVALID_FORFTP					 = -134;	//Error:Check CPL Is InValid For FTP!
const int ERROR_CONTENT_DCP_PKL_FILE_NO_EXIST 						 = -135;	//Error:PKL File(%s) no exist!
const int ERROR_CONTENT_DCP_PKL_FILE_COPY_FAILED 					 = -136;	//PKL File copy Failed 
const int ERROR_CONTENT_DCP_PARAM_PKLNAME_IS_NULL_STR 				 = -137;	//Error:Param pklName is Null string!
const int ERROR_CONTENT_DCP_ASSETMAP_IS_NO_EXIST 					 = -138;	//Error:Assetmap(%s) is no exist!
const int ERROR_CONTENT_DCP_PARAM_ASSETMAPNAME_IS_NULL_STR 			 = -139;	//Error:Param AssetmapName is Null string!
const int ERROR_CONTENT_DCP_FILE_COPY_FAILED 				         = -140;	//File copy Failed
const int ERROR_CONTENT_DCP_FILECOPYNAME_IS_NULL 			         = -141;	//Error:FileCopyName is NULL
const int ERROR_CONTENT_DCP_CPL_FILE_PATH_IS_NULL 			         = -142;	//Error:CPL File Path Is NULL
const int ERROR_CONTENT_DCP_CPL_FILE_IS_NO_EXIST 					 = -143;	//Error:CPL FILE is no exist!
const int ERROR_CONTENT_DCP_MXF_FILE_PATH_IS_NULL 					 = -144;	//Error:mxf File Path Is NULL!
const int ERROR_CONTENT_DCP_MXF_FILE_IS_NO_EXIST 					 = -145;	//Error:mxf File is no exist
const int ERROR_CONTENT_DCP_MXF_FILE_COPY_IS_UNFINISHED 			 = -146;	//Error:mxf File copy is unfinished
const int ERROR_CONTENT_DCP_REBUILD_ASSETMAP_FILE_FAILED 			 = -147;	//Error ReBuild_Assetmap file FAILED
const int ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL					 = -148;	//Error:PKL AssetList is null!
const int ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL				 = -149;	//Error:Pkl AssetFile Path Is NULL!
const int ERROR_CONTENT_DCP_FILE_NOT_EXIST							 = -150;	//Error:file NOT exist
const int ERROR_CONTENT_DCP_PKL_PKLNAME_ERROR						 = -151;	//pklName %s Error!
const int ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL						 = -152;	//CODE_BP_DCPCOPYCANCEL
const int ERROR_CONTENT_DCP_DCPCOPYCANCEL							 = -153;	//CODE_DCPCOPYCANCEL	,�Ƕϵ�����mode

const int ERROR_CONTENT_HASH_HASHCHECK_NO_START						 = -200;	//error:DCP Hash Check is NO START.
const int ERROR_CONTENT_HASH_SHA1_CHECK_ERROR  					     = -201;	//"SHA1 CHECK ERROR!"
const int ERROR_CONTENT_HASH_SHA1_CHECK_CANCEL 						 = -202;	//SHA1 CHECK CANCEL!

//@author zhangmiao@oristartech.com
//@date [2013-08-12]
//@brief ��Ҫ������Satelliteר�����ǽӿڵĴ������
//@new
const int ERROR_CONTENT_DCP_SEARCH_SATE_DCP_LIST 					 = -401;	//error: SearchSatDcpList()
const int ERROR_CONTENT_DCP_SATE_INIT 			    		         = -402;	//error:C_Satellite::Init()
const int ERROR_CONTENT_DCP_SATE_CREATE_CERT_CONNECT 			     = -403;	//error:C_Satellite::CreateCertConnect()
const int ERROR_CONTENT_DCP_SATE_GET_DCP_LIST 						 = -404;	//error:C_Satellite::GetDcpList()
const int ERROR_CONTENT_DCP_SATE_GET_FTP_DCPINFO_BY_ID 			     = -405;	//error:C_Satellite::GetFTPDCPInfoById()
const int ERROR_CONTENT_DCP_SATE_SET_FTP_DOWNLOAD_FINISHED 			 = -406;	//error:C_Satellite::SetFTPDownLoadFinished()
//@modify end;

//#define TMS2_CM_ERRCODE
#ifdef TMS2_CM_ERRCODE
#define CMTT(error) ContentErrorCodeToTmsErrorCode(error);
#define CMTT2(error) ContentErrorCodeToTmsErrorCode(error)
int ContentErrorCodeToTmsErrorCode(int error);
#else
#define CMTT(error) (error);
#define CMTT2(error) (error)
#endif // TMS2_CM_ERRCODE

//#define WRITE_ELOG 
#ifdef WRITE_ELOG
#define WELOG(str) WriteErrorLog(str);
int WriteErrorLog( const std::string& strError );
#else
#define WELOG(str) 
#endif // WRITE_ELOG



#define _BP_DCPCOPYCANCEL_

#endif
