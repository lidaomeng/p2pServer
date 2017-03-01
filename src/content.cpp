/*
*@file:content.cpp
*@brief:����dcp�� ���غ�ftp
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
//#ifndef __DCPINFO_H__
//#include "DcpInfo.h"
//#endif
#ifndef __CONTENT_H__
#include "content.h"
#endif

using namespace Content;
using namespace Content::Dcp;
using namespace std;

#ifndef WIN32
#include <pthread.h>
pthread_mutex_t work_mutex;
#else
HANDLE hMutex = CreateMutex(NULL, FALSE, (LPCWSTR)"DCPContent");
#endif

//���塢����ȫ�ֱ���������֧�ֻ�ȡ���ȡ�
std::string g_sCopyDcp_PklUuidName;//������pklUUID��
std::string g_sCopyDcp_Copying_FileName;//���ڿ������ļ���
int g_flag_DcpCopyStop = 0;//��־λ��1=ȡ������DCP
unsigned long long g_Dcp_AllFile_Size = 0ull;//Dcp�����ļ��Ĵ�С��
unsigned long long g_hasDcpCopied_Size = 0ull;//�Ѿ�������dcp���Ĵ�С��
std::map< std::string , FileCopyInfo > g_map_FileCopyInfo;//ӳ������������ļ�����״̬��Ϣ���ļ�·������FileCopyInfo��
cftp::FtpClient g_FtpClient;//ftp�ͻ���

//@author zhangmiao@oristartech.com
//@date [2012-07-10]
//@brief ��������hashУ���ȫ�ֱ���
//@new
extern int g_flag_SHA1CheckStop;
extern std::string g_sHashDcp_PklUuidName;
extern std::string g_sHashDcp_Checking_FileName;
extern unsigned long long g_AllFile_Size;
extern unsigned long long g_hasHashChecked_Size;
//@modify end;

std::map<std::string,CPL_CONTENT_KIND> mapCPL_ContentKind;

void init_maplist()
{
	mapCPL_ContentKind["feature"]=feature;
	mapCPL_ContentKind["trailer"]=trailer;
	mapCPL_ContentKind["test"]=test;
	mapCPL_ContentKind["teaser"]=teaser;
	mapCPL_ContentKind["rating"]=rating;
	mapCPL_ContentKind["advertisement"]=advertisement;
	mapCPL_ContentKind["short"]=shorts;
	mapCPL_ContentKind["transitional"]=transitional;
	mapCPL_ContentKind["psa"]=psa;
	mapCPL_ContentKind["policy"]=policy;
}

//@author zhangmiao@oristartech.com
//@date [2013-08-12]
//@brief ��Ҫ������Satelliteר�����ǽӿڵĴ������
//@new
#include "C_SatXmlParser.h"
#include "C_Satellite.h"
//��IP��ַ��ָ��DCPר�����ǽ����豸�У�����FTP������ȫ����DCP����
int SearchSatDcpList( const string &Ip , vector<Content::AssetmapInfo> &dcpList , string& userName , string& passWd , string& error , unsigned short usPort )
{
	int ret(0);
	vector<string> vTmp;
	vTmp.clear();

	C_Satellite Sate;

	//��ʼ�����������ǽ����豸tcp����
	//unsigned short usPort = 20080;
	ret = Sate.Init( Ip.c_str() , usPort );
	if( ret != 0 )
	{
		printf("Error:C_Satellite::Init() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::Init() , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_INIT);
		return ret;
	}

	//����������֤����
	ret = Sate.CreateCertConnect();
	if( ret != 0 )
	{
		printf( "Error:C_Satellite::CreateCertConnect() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::CreateCertConnect() , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_CREATE_CERT_CONNECT);
		return ret;
	}

	//���ӰƬ��Ϣ�б�
	vector< Satellite::DCPInfo > vSatDcpList;
	ret = Sate.GetDcpList( vSatDcpList );
	if( ret != 0 )
	{
		printf( "Error:C_Satellite::GetDcpList() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::GetDcpList() , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_GET_DCP_LIST);
		return ret;
	}
	//���ӰƬ�����ڣ���ֱ�ӷ��ؼ��ɡ�
	int nDcpListCount =  vSatDcpList.size();
	if ( nDcpListCount == 0 )
	{
		printf("Warning:C_Satellite::GetDcpList(),nDcpListCount = 0 !\n");
		return 0;
	}

	//��ö�ӦӰƬFTP��Ϣ
	vector< Satellite::FTP_DCPInfo > vtFTP_DCPInfoList;
	for ( int i = 0; i < nDcpListCount; i++ )
	{
		Satellite::FTP_DCPInfo aFTP_DCPInfo;

		Satellite::DCPInfo& rDCPInfo = vSatDcpList[i];
		string& rPklUuid = rDCPInfo.PklUuid;

		//���˵��ظ���PklUuid
		int result = count(vTmp.begin(),vTmp.end(), rPklUuid );
		if( result != 0 ) 
		{
			printf( "Warning Sate:PklUuid Repeat:%s!\n", rPklUuid.c_str() );
			continue;
		}
		else
		{
			vTmp.push_back(rPklUuid);
		}

		ret = Sate.GetFTPDCPInfoById( rPklUuid , aFTP_DCPInfo );
		if( ret != 0 )
		{
			printf( "Error:C_Satellite::GetFTPDCPInfoById(), Uuid= %s !\n" , rPklUuid.c_str() );

			char buffer[BUF_SIZE]="";
			sprintf( buffer , "Error:C_Satellite::GetFTPDCPInfoById() , Uuid= %s !\n" , rPklUuid.c_str() );
			error = buffer + error;
			WELOG(buffer);

			continue;
		}

		vtFTP_DCPInfoList.push_back( aFTP_DCPInfo );
	}

	int nFTP_DCPInfoListCount = vtFTP_DCPInfoList.size();
	if ( ( nFTP_DCPInfoListCount < nDcpListCount ) && ( nFTP_DCPInfoListCount != 0 ) )
	{
		printf("Warning Sate:nFTP_DCPInfoListCount < nDcpListCount!\n");
	}
	else if( ( nFTP_DCPInfoListCount == 0 ) && ( nDcpListCount != 0 ) )
	{
		printf("error:Sate vtFTP_DCPInfoList = 0 , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:Sate vtFTP_DCPInfoList = 0 , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_GET_FTP_DCPINFO_BY_ID);
		return ret;
	}

	//add:���������豸ftp�ĵ�¼��user��password
	if ( nFTP_DCPInfoListCount > 0 )
	{
		string ip, username, password, dir;
		string port = "21";

		cftp::FtpClient ftp;
		//��:url = ftp://zhangm:123456@127.0.0.1/dcproot/
		Satellite::FTP_DCPInfo& rFTP_DCPInfo = vtFTP_DCPInfoList[0];
		string sFTP_URL_PATH = rFTP_DCPInfo.ftp_url_path;

		int result = 0;
		result = ftp.ParseUrl( sFTP_URL_PATH , ip, username, password, port, dir);
		if( result == cftp::NOANYERROR )
		{
			userName = username;
			passWd	= password;
		}
	}
	
	//���������豸ftp�ĵ�¼���û��������� end
	

	//����FTP�������ϵ�DCP��
	vTmp.clear();

	for ( int j = 0; j < nFTP_DCPInfoListCount; j++ )
	{
		Satellite::FTP_DCPInfo& rFTP_DCPInfo = vtFTP_DCPInfoList[j];
		string sFTP_URL_PATH = rFTP_DCPInfo.ftp_url_path;

		//���˵��ظ���FTP_URL_PATH
		int result = count( vTmp.begin(),vTmp.end(), sFTP_URL_PATH );
		if( result != 0 ) 
		{
			printf( "Warning Sate FTP_URL_PATH Repeat:%s!\n", sFTP_URL_PATH.c_str() );
			continue;
		}
		else
		{
			vTmp.push_back(sFTP_URL_PATH);
		}
		
		ret = SearchFtpDcpList( sFTP_URL_PATH , dcpList , error );
		if( ret != 0 )
		{
			printf( "Warning Sate:SearchFtpDcpList(),error FTP_URL= %s !\n" , sFTP_URL_PATH.c_str() );

			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Warning Sate:error,SearchFtpDcpList(),FTP_URL= %s !\n" , sFTP_URL_PATH.c_str() );
			error = buffer + error;
			WELOG(buffer);
			
			continue;
		}
	}

	int nSatDCPListCount = dcpList.size();
	if ( ( nFTP_DCPInfoListCount != 0 ) && ( nSatDCPListCount != 0 ) )
	{
		if ( nSatDCPListCount < nFTP_DCPInfoListCount )
		{
			printf( "Warning Sate:SearchSatDcpList(), IP = %s , nSatDCPListCount < nFTP_DCPInfoListCount !\n" , Ip.c_str() );
		}
		ret = 0;
	}
	else if ( ( nFTP_DCPInfoListCount != 0 ) && ( nSatDCPListCount == 0 ) )
	{
		printf( "Error Sate:SearchSatDcpList() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer,"Error Sate:SearchSatDcpList() , IP = %s !\n" , Ip.c_str() );
		WELOG(buffer);
		error = buffer + error;
		ret = CMTT(ERROR_CONTENT_DCP_SEARCH_SATE_DCP_LIST);
	}

	return ret;
}

//��ӦӰƬ��������������status = 1����ʾӰƬ������ϡ�status = 0����ʾ����ʧ�ܡ�
int Sate_SetFTPDownLoadFinished( const string& Ip , const string& PklUuid , const int Status , string& error )
{
	int ret(0);
	
	C_Satellite Sate;

	//��ʼ�����������ǽ����豸tcp����
	unsigned short usPort = 20080;
	ret = Sate.Init( Ip.c_str() , usPort );
	if( ret != 0 )
	{
		printf("Error:C_Satellite::Init() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::Init() , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_INIT);
		return ret;
	}
	
	//����������֤����
	ret = Sate.CreateCertConnect();
	if( ret != 0 )
	{
		printf( "Error:C_Satellite::CreateCertConnect() , IP = %s !\n" , Ip.c_str() );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::CreateCertConnect() , IP = %s !\n" , Ip.c_str() );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_CREATE_CERT_CONNECT);
		return ret;
	}

	//��ӦӰƬ������ϣ����status = 1����ʾӰƬ������ϡ�status = 0����ʾ����ʧ�ܡ�
	ret = Sate.SetFTPDownLoadFinished( PklUuid , Status );
	if( ret != 0 )
	{
		printf( "Error:C_Satellite::SetFTPDownLoadFinished() , IP = %s , PklUuid = %s , Status = %d !\n" , Ip.c_str() , PklUuid.c_str() , Status );

		char buffer[BUF_SIZE]="";
		sprintf( buffer , "Error:C_Satellite::SetFTPDownLoadFinished() , IP = %s , PklUuid = %s , Status = %d !\n" , Ip.c_str() , PklUuid.c_str() , Status );
		error = buffer + error;
		WELOG(buffer);

		ret = CMTT(ERROR_CONTENT_DCP_SATE_SET_FTP_DOWNLOAD_FINISHED);
		return ret;
	}

	return ret;
}
//@modify end;

int SearchFtpDcpList( const std::string &ftpUrl, std::vector<AssetmapInfo> &dcpList ,std::string& error )
{
	//@author zhangmiao@oristartech.com
	//@date [2013-07-18]
	//@brief ��Ҫ������FTP������صĴ�����룬���ڽ��assetmap �ļ��ظ����ֵ����⡣
	//@new
	vector<string> vTmp;
	vTmp.clear();
	vector<AssetmapInfo>::size_type nAssetmapNum = 0;
	nAssetmapNum = dcpList.size();
	if ( nAssetmapNum != 0 )
	{	
		for ( vector<AssetmapInfo>::size_type i=0; i<nAssetmapNum; i++ )
		{
			AssetmapInfo& aAssetMapInfo = dcpList[i];
			string& assetmapUuid = aAssetMapInfo.assetmapUuid;
			int result = 0;
			result = count( vTmp.begin() , vTmp.end() , assetmapUuid );
			if( result != 0 ) 
			{
				continue;
			}
			else
			{
				vTmp.push_back(assetmapUuid);
			}
		}
	}
	//@modify end;

	string url = ftpUrl;
	cftp::FtpClient ftp;
	//log in
	int result = ftp.LogIn(ftpUrl);
	if(result != cftp::NOANYERROR)
		return CMTT2(result);

	string AssetMapFileNameSet[4]  = { "ASSETMAP","assetmap","ASSETMAP.xml","assetmap.xml" };
	for ( int nAssetMapFNCount = 0; nAssetMapFNCount < 4; nAssetMapFNCount++ )
	{
		string filename = AssetMapFileNameSet[nAssetMapFNCount];

		std::vector<cftp::FileInformation> vfileList;
		result = listFilesOnFTP( ftp, "" ,vfileList ,filename );
		if( result != cftp::NOANYERROR )
		{
			//@author zhangmiao@oristartech.com
			//@date [2015-10-15]
			//@brief listFilesOnFTP()����û���ҵ�ָ�����ļ�ʱ����ֵ��100������Ը��ļ���������һ����ص��ļ���
			//@new
			if( result == 100 )
			{
				string errorLog = "\nWarning:listFilesOnFTP(),n1,Return Value is 100,no found file:" + filename+"!\n";
				error += errorLog;
				WELOG(errorLog);

				continue;
			}
			//@modify [2015-10-15] end;

			//old code:return CMTT2(result);
			int nRet = 0;
			nRet = ftp.ReLogIn();
			if( nRet != cftp::NOANYERROR )
			{
#ifdef _TEST_
				printf("\nError:FtpClient ReLogIn(),n1!\n" );
#endif

				string errorLog = "\nError:FtpClient ReLogIn(),n1!\n";
				error += errorLog;
				WELOG(errorLog);

				nRet = ftp.LogIn(ftpUrl);
				continue;
			}

			//@author zhangmiao@oristartech.com
			//@date [2015-10-15]
			//@brief FTP ReLogIn()���µ�¼�󣬱������»ָ����õ�ǰ����Ŀ¼��
			//@new
			string ip, userName, passwd, sWorkDir;
			string sPort = "21";
			nRet = ftp.ParseUrl( url , ip , userName , passwd , sPort , sWorkDir );
			if( nRet != cftp::NOANYERROR )
			{
				continue;
			}
			
			if( !( sWorkDir.empty() || sWorkDir == "/" ) )
			{
				nRet = ftp.SetWorkDir(sWorkDir);
				if( nRet != cftp::NOANYERROR )
				{
					string errorLog = "\nError:FtpClient SetWorkDir(),n1,"+ sWorkDir+"!\n";
					error += errorLog;
					WELOG(errorLog);

					continue;
				}
			}
			//@modify [2015-10-15] end;

			nRet = listFilesOnFTP( ftp , "" ,vfileList ,filename );
			if( nRet != cftp::NOANYERROR )
			{
				//@author zhangmiao@oristartech.com
				//@date [2015-10-15]
				//@brief listFilesOnFTP()����û���ҵ�ָ�����ļ�ʱ����ֵ��100������Ը��ļ���������һ����ص��ļ���
				//@new
				if( result == 100 )
				{
					string errorLog = "\nWarning:listFilesOnFTP(),n2,Return Value is 100,no found file:" + filename+"!\n";
					error += errorLog;
					WELOG(errorLog);

					continue;
				}
				//@modify [2015-10-15] end;

				nRet = ftp.ReLogIn();

				//@author zhangmiao@oristartech.com
				//@date [2015-10-15]
				//@brief FTP ReLogIn()���µ�¼�󣬱������»ָ����õ�ǰ����Ŀ¼��
				//@new
				if( nRet == cftp::NOANYERROR )
				{
					if( !( sWorkDir.empty() || sWorkDir == "/" ) )
					{
						nRet = ftp.SetWorkDir( sWorkDir );
						if( nRet != cftp::NOANYERROR )
						{
							string errorLog = "\nError:FtpClient SetWorkDir(),n2,"+ sWorkDir+"!\n";
							error += errorLog;
							WELOG(errorLog);

							nRet = ftp.LogIn( ftpUrl );

							continue;
						}
					}
				}
				else 
				{
#ifdef _TEST_
					printf("\nError:FtpClient ReLogIn(),n2!\n" );
#endif

					string errorLog = "\nError:FtpClient ReLogIn(),n2!\n";
					error += errorLog;
					WELOG(errorLog);

					nRet = ftp.LogIn(ftpUrl);
				}
				//@modify [2015-10-15] end;

				continue;
			}
		}

		int assetmapN = 0;
		assetmapN = vfileList.size();
		for ( int i=0; i<assetmapN; i++ )
		{
			cftp::FileInformation FileInfo = vfileList[i];
			string name = FileInfo.name; //��ȡ�ļ���;  �������·�������൱��ftpurl�ϵ�·�����磺amovie/ASSETMAP

			string ip, userName, passwd, dir;
			string port = "21";

			//��:url = ftp://zhangm:123456@127.0.0.1/dcproot/
			result = ftp.ParseUrl(url, ip, userName, passwd, port, dir);
			if(result != cftp::NOANYERROR)
				return CMTT2(result);

			//string URL_PFX = url.substr(0,end);
			if(filename.compare(name.substr( name.rfind( '/' )+1 ))==0)
			{
				//ȡ��name��Ŀ¼����,��;amovie/
				string sFilePath;
				if ( name.rfind( '/' )!= string::npos )
				{
					sFilePath = name.substr( 0,name.rfind( '/' )+1);
				}
				//��ftpurl��Ŀ¼������name��Ŀ¼���֣���ϳ�����������Ŀ¼
				//��:url = ftp://zhangm:123456@127.0.0.1/dcproot/  ��dir='/dcproot/' sFilePath='amovie/'
				dir += sFilePath;

				string sFileUrl = ftpUrl + name;
#ifdef _TEST_
				printf("\nFileUrl= %s\n" ,sFileUrl.c_str());
#endif
				//������Ӧ���ļ�����ǰĿ¼��
				result = ftp.TransportFile( dir  , "." , name.substr( name.rfind( '/' )+1) );
				if(result != cftp::NOANYERROR)
					return CMTT2(result);

				//����ASSETMAP�ļ�������Ŀ¼����assetmapUuidΪ���֡�
				AMInfo_t aAMInfo;
				int ret = 0;
				{
					DcpInfo dcp;
					ret = dcp.Parse_Assetmap_New( string(".")+DIR_SEPCHAR+filename, aAMInfo , error );
					if( ret == -1 )
					{
						char buffer[BUF_SIZE]="";
						//@author zhangmiao@oristartech.com
						//@date 2015-03-09 
						//@brief �������FTP Assetmap�ļ�ʧ�ܣ�����Դ��󣬼���������һ��
						//sprintf( buffer,"parseAssetmap error!:%s\n" , (string(".")+DIR_SEPCHAR+filename).c_str() );
						sprintf( buffer,"FTP parseAssetmap error:%s!\n" , sFileUrl.c_str() );
						
						error = buffer;
						
						//printf( "parseAssetmap error!:%s\n" , (string(".")+DIR_SEPCHAR+filename).c_str() );
						printf( "FTP parseAssetmap error:%s!\n" , sFileUrl.c_str() );
						
	
						//return -1;
						WELOG(error)
						//return CMTT2(ERROR_CONTENT_DCP_PARSE_ASSETMAP_ERROR);

						ret = remove( filename.c_str() );
						continue;
						//@modify(2015-03-09)end;
					}
				}

				// delete old ASSETMAP 
				//ret = system( (RMDIR + string(".\ASSETMAP")).c_str() );
				ret = remove( filename.c_str() );
				string assetmapUuid = ((aAMInfo).aAssetmap_Info).assetmapUuid;

				//@author zhangmiao@oristartech.com
				//@date [2013-07-18]
				//@brief ��Ҫ������FTP������صĴ�����룬���ڽ��assetmap �ļ��ظ����ֵ����⡣
				//@new
				int result = count(vTmp.begin(),vTmp.end(), assetmapUuid );
				if( result != 0 ) 
				{
					printf( "Warning AssetmapUuid Repeat:%s!\n", sFileUrl.c_str() );
					continue;
				}
				else
				{
					vTmp.push_back(assetmapUuid);
				}
				//@modify end;

				//Parameters Check
				string dest = ".";
				string sFilmFilePath = dest;
				if( sFilmFilePath.rfind(DIR_SEPCHAR) != sFilmFilePath.length()-1 )
					sFilmFilePath.append( 1, DIR_SEPCHAR );

				size_t pos;
				if ( ( pos = assetmapUuid.find("urn:uuid:") ) != assetmapUuid.npos && pos==0 )
				{
					sFilmFilePath = sFilmFilePath + assetmapUuid.substr(9);
				}
				else
				{
					sFilmFilePath = sFilmFilePath + assetmapUuid;
				}
				sFilmFilePath.append( 1, DIR_SEPCHAR );

				if ( sFilmFilePath.length()==0 )
				{
					error = "Error:FilmFilePath is NULL!\n";
					//errno = -2;
					errno = ERROR_CONTENT_DCP_FILM_FILE_PATH_ISNULL;
					WELOG(error)
					return CMTT2(errno);
				}

				string  Dest = sFilmFilePath;
				ret = MakeDirectory(Dest);
				if(ret!=0)
				{
					error = "Error:Make Directory fail.\n";
					WELOG(error)
					ret = CMTT2(ERROR_CONTENT_DCP_MAKE_DIR_FAIL);
					return ret;
				}

				//����ASSETMAP�ļ� 
				result = ftp.TransportFile( dir , sFilmFilePath, name.substr( name.rfind( '/' )+1));
				if(result != cftp::NOANYERROR)
					return CMTT2(result);

				//����dcp�����ļ�    
				vector<asset_assetmap_t> asset = ((aAMInfo).aAssetmap_Info).asset;
				vector<asset_assetmap_t>::size_type nSize = asset.size();
				for ( vector<asset_assetmap_t>::size_type j=0;j<nSize;j++ )
				{
					asset_assetmap_t asset_assetmap = asset[j];
					//@author zhangmiao@oristartech.com
					//@date [2013-07-18]
					//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
					//@new
					/*string Path = asset_assetmap.path;
					if ( Path.rfind(".mxf")== string::npos )
					{
						result = ftp.TransportFile( dir , sFilmFilePath ,Path.substr( Path.rfind( DIR_SEPCHAR )+1) );
						if(result != cftp::NOANYERROR)
							return CMTT2(result);
					}*/
					string Path = asset_assetmap.Path_NotFullPath;
					if ( Path.rfind(".mxf")== string::npos && Path.rfind(".png")== string::npos && Path.rfind("/")== string::npos )
					{
						result = ftp.TransportFile( dir , sFilmFilePath , Path );
						if(result != cftp::NOANYERROR)
							return CMTT2(result);
					}
					//@modify end;
				}
				string srcPath = sFilmFilePath;
				ret = SearchDcpList(srcPath,dcpList,error);
				if(ret!=0)
				{
					char buffer[BUF_SIZE]="";
					sprintf( buffer,"error:SearchDcpList() = %s!\n",srcPath.c_str() );
					error = buffer;
					printf("error:SearchDcpList() = %s!\n",srcPath.c_str());

					WELOG(error)
					ret = CMTT2(ERROR_CONTENT_DCP_SEARCH_DCP_LIST);
					//@author zhangmiao@oristartech.com
					//@date 2015-03-09 
					//@brief �������FTP�����ص�Dcp���е��ļ�ʧ�ܣ�����Դ��󣬼���������һ��
					//return ret;
					continue;
					//@modify(2015-03-09)end;
				}

				//�����趨·���� dir
				int nAMInfoCount = dcpList.size();
				for ( int j=0; j< nAMInfoCount; j++ )
				{
					AssetmapInfo& aAssetMapInfo = dcpList[j];
					if (assetmapUuid==aAssetMapInfo.assetmapUuid)
					{
						aAssetMapInfo.dcpPath = dir;
						string assetmapName = aAssetMapInfo.assetmapName;
						aAssetMapInfo.assetmapName = dir + assetmapName.substr(assetmapName.rfind(DIR_SEPCHAR)+1);
						vector<PklInfo>& pklList = aAssetMapInfo.pklList;
						int pklN = pklList.size();
						for( int k=0; k<pklN; k++ )
						{
							PklInfo& aPkl = pklList[k];	
							string pklName = aPkl.pklName;
							aPkl.pklName = dir + pklName.substr(pklName.rfind(DIR_SEPCHAR)+1);
							vector<CplInfo>& cplList = aPkl.cplList;
							vector<Mxf_FileInfo>& mxfList = aPkl.mxfList;
							int cplCount = cplList.size();
							for ( int m = 0; m < cplCount; m++ )
							{
								CplInfo& cpl = cplList[m];
								string cplName  = cpl.cplName;
								cpl.cplName =  dir + cplName.substr(cplName.rfind(DIR_SEPCHAR)+1);
							}
							int mxfCount = mxfList.size();
							for ( int m = 0; m < mxfCount; m++ )
							{
								Mxf_FileInfo& aMxfInfo = mxfList[m];
								string mxfName = aMxfInfo.fileName;
								aMxfInfo.fileName =  dir + mxfName.substr(mxfName.rfind(DIR_SEPCHAR)+1);
							}

							//@author zhangmiao@oristartech.com
							//@date [2013-07-18]
							//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
							//@new
							vector<OtherFileInfo>& fileInfoList = aPkl.fileInfoList;
							int otherCount = fileInfoList.size();
							for ( int m = 0; m < otherCount; m++ )
							{
								OtherFileInfo& aFileInfo = fileInfoList[m];
								string originalfileName = aFileInfo.originalfileName;
								aFileInfo.fileName =  dir + originalfileName;
							}
							//@modify end;
						}
					}
				}

				//���cpl�ļ���Ч״̬
				/*ret = CheckCPLIsValid4Ftp( dir, assetmapUuid, ftp, dcpList );
				if( ret != 0 )
				{
					error = "Error:Check CPL Is InValid For FTP!\n";
					WELOG(error)
					ret = CMTT2(ERROR_CONTENT_DCP_CHECKCPL_INVALID_FORFTP);
					return ret;
				}*/

				//ɾ�� ASSSETMAP Ŀ¼
				if( ( ret = access( Dest.c_str() , F_OK ) ) == 0 )     
				{
#ifdef _TEST_
					printf( "File Directory already exists :%s\n", Dest.c_str() );
#endif
					ret = system( (RMDIR + Dest).c_str() );
					if( ret == 0 )
					{
#ifdef _TEST_
						printf( "\nDirectory deleted :%s\n", Dest.c_str() );
#endif
						errno = 0;
					}
					else
					{
#ifdef _TEST_
						perror("\nUnable to delete directory\n");
#endif
						error = "\nUnable to delete ASSSETMAP directory\n";
						
						WELOG(error)
					}
				}
			}

		}
	}
		
	//@author zhangmiao@oristartech.com
	//@date [2014-07-18]
	//@brief ��ҪFTP����ʱЭ������(quit)��Ӧ������Ϣ���ȶ�������
	//@old ��ǰ����
	/*result = ftp.LogOut();
	if(result != cftp::NOANYERROR)
		return CMTT2(result);*/
	//@new
	ftp.LogOut();
	//@modify end;

	return 0;
}
int SearchDcpList( const string &path, vector<AssetmapInfo> &dcpList ,std::string& error )
{
	init_maplist();

	int ret(0);
	DcpInfo dcp;
	vector< AMInfo > aAMInfo_Vt;
	ret = dcp.CreateDCPInfo( path , error );
	if( ret!=0 ) 
	{
#ifdef _TEST_
		printf("error:CreateDCPInfo()!\n");
#endif
		//error = "error:CreateDCPInfo()!\n";
		error = "����δ�ҵ���Ч��DCP����";
		WELOG(error)
		ret = CMTT(ERROR_CONTENT_DCP_CREATE_DCPINFO_ERROR);
		return ret;
	}
	else if ( ret==0 )
	{
		aAMInfo_Vt = dcp.vAMInfo_t;
		int nAMInfoCount = aAMInfo_Vt.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AssetmapInfo aAssetMapInfo;
			AMInfo_t aAMInfo = aAMInfo_Vt[i];
			//assetmap:uuid
			aAssetMapInfo.assetmapUuid = aAMInfo.aAssetmap_Info.assetmapUuid;
			//assetmap �ļ��������ִ�Сд
			aAssetMapInfo.assetmapName = aAMInfo.assetmapName;
			///dcp����·���������Ǵ��̾���·����ftp url
			aAssetMapInfo.dcpPath = aAMInfo.dcpPath;
			//����dcp�����ļ���С���ܺͣ�asssetmap���ļ���С֮�͡�
			long long allFileLength=0;
			vector<asset_assetmap_t>& aAssetVt = aAMInfo.aAssetmap_Info.asset;
			int nAssetVtCount = aAssetVt.size();
			for ( int j=0; j< nAssetVtCount; j++ )
			{
				//assetmap�ļ���Length����ǿ�ѡ�ģ����ĳ��Asset�ļ�û��Length��ǣ����Լ����ļ����ȡ����޷���֤���㲻һ���ܳɹ���
				//��Ҫ��pkl��cpl�ļ��ĳ���Length��ǣ�����û�У����Ե���dcp�ܳ����������
				unsigned long long nTmpFileLength = aAssetVt[j].Length;
				if ( nTmpFileLength == 0 )
				{
					string sTmpFilePath = aAssetVt[j].path;
					struct stat tmpstsrc;
					if( lstat( sTmpFilePath.c_str(), &tmpstsrc ) == 0 )
					{
						nTmpFileLength = tmpstsrc.st_size;
						aAssetVt[j].Length = nTmpFileLength;
					}
					else
						nTmpFileLength = 0;

					allFileLength += nTmpFileLength;
				}
				else 
					allFileLength += aAssetVt[j].Length;
			}
			aAssetMapInfo.dcpLength = allFileLength;
			//assetmap�ļ���С,assetmapLength.
			struct stat stsrc;
			if( lstat( aAssetMapInfo.assetmapName.c_str(), &stsrc ) == 0 )
				aAssetMapInfo.assetmapLength = stsrc.st_size;
			else{
#ifdef _TEST_
				printf("lstat %s is err:%s!\n",aAssetMapInfo.assetmapName.c_str(),strerror(errno));
#endif
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"lstat %s is error:%s!\n",aAssetMapInfo.assetmapName.c_str(),strerror(errno) );
				error = buffer;
				break;
			}
			//-------------------
			vector< dcp_Info_t > vDcp = aAMInfo.vDcp;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				PklInfo aPklInfo;

				dcp_Info_t dcp = vDcp[j];
				pkl_Info_t aPkl = dcp.aPkl;
				aPklInfo.pklUuid = aPkl.uuid;
				//----zhangmiao:begin---2012-09-24----
				//Content::PklInfo�ṹ�����������¼������ԣ�AnnotationText��IssueDate��Issuer��Creator��
				aPklInfo.AnnotationText = aPkl.annotationText;
				aPklInfo.IssueDate      = aPkl.issueDate;
				aPklInfo.Issuer         = aPkl.issuer;
				aPklInfo.Creator        = aPkl.creator;
				//----zhangmiao:end-----2012-09-24----
				aPklInfo.allFileLength = aPkl.allFileLength;
				//����pkl�ļ��Ĵ�С�����֡�
				int nAssetVtSize = aAssetVt.size();
				for ( int k=0; k< nAssetVtSize; k++ )
				{
					if ( aPklInfo.pklUuid == aAssetVt[k].uuid )
					{
						aPklInfo.pklName =  aAssetVt[k].path;
						aPklInfo.pklLength = aAssetVt[k].Length;
#ifdef _TEST_
						printf( "pklName:%s,pklLength=%llu\n", aPklInfo.pklName.c_str() ,aPklInfo.pklLength );
#endif
						break;
					}
				}

				//@author zhangmiao@oristartech.com
				//@date [2015-04-17]
				//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
				//@new
				aPklInfo.nFilmPackageFormat = aPkl.nFilmPackageFormat;
				//@modify end;

				//PklInfo��create cpl list
				int cplCount = dcp.vCpl.size();
				vector< cpl_Info_t > vCpl = dcp.vCpl;
				for ( int k=0; k< cplCount; k++ )
				{
					CplInfo cpl;
					cpl.is3D = 0;
					cpl.cplUuid = vCpl[k].uuid;
					cpl.cplName = vCpl[k].path;
					cpl.type    = mapCPL_ContentKind[vCpl[k].contentKind];
					//----zhangmiao:begin---2012-09-25----
					//Content::CplInfo�ṹ�����������¼������ԣ�cplTitle��cplIssueDate��cplssuer��cplCreator��
					cpl.cplTitle     = vCpl[k].contentTitleText;
					cpl.cplIssueDate = vCpl[k].issueDate;		//����ʱ��
					cpl.cplssuer     = vCpl[k].issuer;			//������
					cpl.cplCreator   = vCpl[k].creator;
					//----zhangmiao:end-----2012-09-25----
					//mainpicture
					string strEditRate  = "";
					string strFrameRate = "";
					string strKeyid     = "";
					string strSrcAspRat = "";

					long long fEditRate = 24;
					long long duration = 0ll;		//����ʱ�䣬��֡Ϊ��λ;(duration/fEditRate)��Ϊ��
					long long fRate = 0ll;			//frameRate,���frameRateΪ�գ�����editRate����
					float fScreenAspectRatio = 0;	//���ݱ�
					int needKdm = 0;
					int is3D = 0;
					vector< asset_cpl_t > vAsset_Cpl = vCpl[k].asset;
					int nAsset_CPL_Count = vAsset_Cpl.size();
					for ( int m=0; m<nAsset_CPL_Count; m++ )
					{
						asset_cpl_t assetCpl = vAsset_Cpl[m];
						string kind = assetCpl.kind;
						/*if ( kind=="application/x-smpte-mxf;asdcpKind=Picture" 
							|| stricmp(kind.substr(kind.rfind(';')+1).c_str() ,"asdcpKind=Picture")==0 )*/
						if (  kind == "MainPicture" 
						   || kind == "MainStereoscopicPicture" )
						{
							int result = count( cpl.mainPicList.begin(),cpl.mainPicList.end(), assetCpl.uuid );
							if(result==0)
							//��¼UUID
							cpl.mainPicList.push_back(assetCpl.uuid);
							
							//duration
							if ( assetCpl.duration.empty() )
							{
								duration += atoll( assetCpl.intrinsicDuration.c_str() );
							}
							else
							{
								duration += atoll( assetCpl.duration.c_str() );
							}

							//EditRate
							if(assetCpl.editRate.empty())
							{
								fEditRate = 24;
							}
							else
							{
								strEditRate = assetCpl.editRate;
								fEditRate = atoll(strEditRate.substr(0,strEditRate.find(' ')).c_str());
							}
							//fRate
							if ( assetCpl.frameRate.empty() )
							{
								strFrameRate = assetCpl.editRate;
							}
							else
							{
								strFrameRate = assetCpl.frameRate;
							}
							fRate = atoll(strFrameRate.substr(0,strFrameRate.find(' ')).c_str());

							//ScreenAspectRatio
							if ( !assetCpl.screenAspectRatio.empty() )
							{
								strSrcAspRat = assetCpl.screenAspectRatio;
								fScreenAspectRatio = (float)atof(assetCpl.screenAspectRatio.c_str());
							}

							//keyId
							if ( !assetCpl.keyId.empty() )
							{
								strKeyid = assetCpl.keyId;
								needKdm=1;
							}

							//is3D
							if ( kind == "MainStereoscopicPicture" )
							{
								is3D = 1;
							}
						}
						/*else if ( kind=="application/x-smpte-mxf;asdcpKind=Sound" 
							     || stricmp(kind.substr(kind.rfind(';')+1).c_str() ,"asdcpKind=Sound")==0 )*/
						else if ( kind=="MainSound"  )
						{
							int result = count( cpl.mainSoundList.begin(),cpl.mainSoundList.end(), assetCpl.uuid );
							if(result==0)
							//��¼UUID
							cpl.mainSoundList.push_back(assetCpl.uuid);

							//keyId
							if ( !assetCpl.keyId.empty() )
							{
								strKeyid = assetCpl.keyId;
								needKdm=1;
							}

							//@author zhangmiao@oristartech.com
							//@date 2013-12-09
							//@brief ��Ҫ�޸�CplInfo�ṹ�壬����CPL�ֶ�MainSoundLanguage�Ĵ������
							//@new
							//��ʾӰƬ���ԣ��������Դ��룬���cpl�ļ���û�и��ֶΣ���Ĭ�Ϸ���δ֪,"unknown"��
							if ( assetCpl.language == "" )
							{
								//��������ڣ���Ĭ�Ϸ���δ֪,"unknown"��
								cpl.MainSoundLanguage = "unknown";
							}
							else
							{
								cpl.MainSoundLanguage = assetCpl.language;
							}
							//@modify end;
						}

					}

					//@author zhangmiao@oristartech.com
					//@date 2015-05-05
					//@brief ���Ӵ��룬�ж���Ƶ���ͣ�nVideoType��ֵ:1����Mpeg2��0����2D��ʽ��Jpeg2000
					if ( is3D == 1 )
					{
						cpl.nVideoType = VIDEO_TYPE_JPEG2000;
					} 
					else
					{
						//�����2DƬ������һ�����ж�
						string sPicUuid ;
						if ( cpl.mainPicList.size() > 0 )
						{
							sPicUuid  = cpl.mainPicList[0];
						}
						
						VIDEO_TYPE enumVideoType = VIDEO_TYPE_UNKNOWN;

						CheckVideoType( enumVideoType , sPicUuid ,  aPkl.asset );
						
						cpl.nVideoType = enumVideoType;
					}
					//@modify [2015-05-05] end;

					cpl.is3D = is3D;
					cpl.rate = (int)fRate;
					cpl.duration = (int)(duration/fEditRate);
					cpl.Duration_double = (double)duration/fEditRate;    //double���͵�duration������Ϊ��λ
					cpl.ScreenAspectRatio = fScreenAspectRatio;
					cpl.needKdm = needKdm;
					//set cpl�ļ��Ĵ�С
					vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
						string type = aAsset_Pkl.type;
						//if ( (type == "text/xml;asdcpKind=CPL") && (aAsset_Pkl.uuid == "urn:uuid:"+cpl.cplUuid) )
						//@author zhangmiao@oristartech.com
						//@date [2013-09-18]
						//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
						//@new
						//if ( (type == "text/xml;asdcpKind=CPL") && (aAsset_Pkl.uuid == cpl.cplUuid) )
						if ( aAsset_Pkl.uuid == cpl.cplUuid )
						//@modify end;
						{
							cpl.cplLength = aAsset_Pkl.size;
							break;
						}
					}

					//����CPL���Ĵ�С
					vector<string> vTmp;
					long long allFileLength = 0ll;    //CPL����С���ļ������ܺϣ�
					for ( int m = 0; m < nAsset_CPL_Count; m++ )
					{
						asset_cpl_t assetCpl = vAsset_Cpl[m];
						string asset_cpl_Uuid = assetCpl.uuid;
						int result = count(vTmp.begin(),vTmp.end(), asset_cpl_Uuid );
						if(result!=0) continue;
							else   vTmp.push_back(asset_cpl_Uuid);
						for ( int k = 0; k < nSize_Asset_Pkl; k++ )
						{
							asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
							string Uuid_Asset_Pkl = aAsset_Pkl.uuid;
							//if ( Uuid_Asset_Pkl == "urn:uuid:" + asset_cpl_Uuid )
							if ( Uuid_Asset_Pkl == asset_cpl_Uuid )
							{
								allFileLength += aAsset_Pkl.size;
								break;
							}
						}
					}
					cpl.allFileLength = allFileLength;

					//���cpl�ļ���Ч״̬
					bool bIsInvalid  = false;
					bIsInvalid = CheckCPLIsValid( vAsset_Cpl , vAsset_pkl , error );					
					cpl.status = bIsInvalid;

					//���µ�cpl����PklInfo.cplList.
					aPklInfo.cplList.push_back(cpl);
				}
			   
			   //PklInfo�� create mxf list
				vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
				int nSize_Asset_Pkl = vAsset_pkl.size();
				for ( int k=0; k< nSize_Asset_Pkl; k++ )
				{
					Mxf_FileInfo aMxfInfo;
					asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
					string type = aAsset_Pkl.type;
					//@author zhangmiao@oristartech.com
					//@date [2013-09-18]
					//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
					//@old
					/*if ( type == "application/x-smpte-mxf;asdcpKind=Picture" || 
						 type == "application/x-smpte-mxf;asdcpKind=Sound" || 
						 stricmp(type.substr(type.rfind(';')+1).c_str() ,"asdcpKind=Picture")==0 || 
						 stricmp(type.substr(type.rfind(';')+1).c_str() ,"asdcpKind=Sound")==0  )
				      */
					//@new
					if ( type == "application/x-smpte-mxf;asdcpKind=Picture" || 
						 type == "application/x-smpte-mxf;asdcpKind=Sound" || 
						 stricmp(type.substr(type.rfind(';')+1).c_str() ,"asdcpKind=Picture") ==0 || 
						 stricmp(type.substr(type.rfind(';')+1).c_str() ,"asdcpKind=Sound") ==0   || 
						 type == "application/mxf" ||
						 stricmp( type.c_str() ,"application/mxf" ) == 0 )
					//@modify end;
					{
						aMxfInfo.mxfUuid  = aAsset_Pkl.uuid;
						aMxfInfo.fileName = aAsset_Pkl.originalFileName;
						aMxfInfo.length   = aAsset_Pkl.size;
						aPklInfo.mxfList.push_back(aMxfInfo);
					}
				}
#ifdef _TEST_
			   printf( "create mxf list Number:%d\n", (int)aPklInfo.mxfList.size() );
#endif

			   //@author zhangmiao@oristartech.com
			   //@date [2013-07-18]
			   //@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
			   //@new
			   //PklInfo�� create Subtitle list
			   for ( int k = 0; k < nSize_Asset_Pkl; k++ )
			   {
				   OtherFileInfo aOtherInfo;
				   asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
				   string type = aAsset_Pkl.type;
				   //@author zhangmiao@oristartech.com
				   //@date [2013-09-22]
				   //@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
				   //@old
				   /*if ( type == "text/xml;asdcpKind=Subtitle" || 
					   type == "image/png" || 
					   stricmp(type.substr(type.rfind(';')+1).c_str() , "asdcpKind=Subtitle") == 0 || 
					   stricmp(type.c_str() , "image/png") == 0 )*/
				   //@new
				   if ( type == "text/xml;asdcpKind=Subtitle" || 
					    type == "image/png" || 
					   stricmp(type.substr(type.rfind(';')+1).c_str() , "asdcpKind=Subtitle") == 0 || 
					   stricmp(type.c_str() , "image/png") == 0 ||
					   type == "application/ttf" ||
					   stricmp( type.c_str() ,"application/ttf" ) == 0 )
				   //@modify end;
				   {
					   aOtherInfo.Uuid             = aAsset_Pkl.uuid;
					   aOtherInfo.fileName         = aAsset_Pkl.originalFileName;
					   aOtherInfo.originalfileName = aAsset_Pkl.Path_NotFullPath;
					   aOtherInfo.fileLength       = aAsset_Pkl.size;
					   aPklInfo.fileInfoList.push_back(aOtherInfo);
				   }
			   }
#ifdef _TEST_
			   printf( "create fileInfo list Number:%d\n", (int)aPklInfo.fileInfoList.size() );
#endif
			   //@modify end;
			   
			   aAssetMapInfo.pklList.push_back(aPklInfo);
			}
			//dcpList
			dcpList.push_back(aAssetMapInfo);
		}
		//��dcpList���������������1��
		if( dcpList.size() > 1)
		{
		    sort(dcpList.begin(),dcpList.end(),FuncCompDcpList());
		}
	}
	

	return 0;
}
int CheckCPLIsValid( const vector< asset_cpl_t >& vAsset_Cpl , const vector< asset_pkl_t >& vAsset_pkl ,std::string& error )
{
	bool bIsInvalid  = false;
	int nSize_Asset_Pkl = vAsset_pkl.size();
	int nAsset_CPL_Count = vAsset_Cpl.size();
	for ( int m = 0; m < nAsset_CPL_Count; m++ )
	{
		asset_cpl_t assetCpl = vAsset_Cpl[m];
		string asset_cpl_Uuid = assetCpl.uuid;
		for ( int k = 0; k < nSize_Asset_Pkl; k++ )
		{
			asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
			string Uuid_Asset_Pkl = aAsset_Pkl.uuid;
			//if ( Uuid_Asset_Pkl == "urn:uuid:" + asset_cpl_Uuid )
			if ( Uuid_Asset_Pkl == asset_cpl_Uuid )
			{
				string fileName = aAsset_Pkl.originalFileName;
				//����ļ���С,assetLength
				unsigned long long assetLength = 0;
				struct stat stsrc;
				if( lstat( fileName.c_str(), &stsrc ) == 0 )
					assetLength = stsrc.st_size;
				else{
					bIsInvalid = true;
#ifdef _TEST_
					printf("lstat %s is error:%s!\n",fileName.c_str(),strerror(errno));
#endif
					char buffer[BUF_SIZE]="";
					sprintf( buffer,"lstat %s is error:%s!\n",fileName.c_str(),strerror(errno) );
					error += buffer;
					break;
				}
				//�Ƚ��ļ���С
				unsigned long long filesize = aAsset_Pkl.size;
				if ( filesize != assetLength )
				{
					bIsInvalid = true;
					break;
				}
				break;
			}
		}
		//���ֳ������⣬������ֹ��顣
		if ( bIsInvalid == true )
		{
			break;
		}
	}

	return bIsInvalid;
}
int CheckCPLIsValid4Ftp( const string dir , const string assetmapUuid , const cftp::FtpClient& ftp , vector<AssetmapInfo>& dcpList )
{
	int ret(0);
	int nAMInfoCount = dcpList.size();
	for ( int j=0; j < nAMInfoCount; j++ )
	{
		AssetmapInfo& aAssetMapInfo = dcpList[j];
		if ( assetmapUuid == aAssetMapInfo.assetmapUuid )
		{
			vector<PklInfo>& pklList = aAssetMapInfo.pklList;
			int pklN = pklList.size();
			for( int k=0; k < pklN; k++ )
			{
				PklInfo& aPkl = pklList[k];	
				vector<CplInfo>& cplList = aPkl.cplList;
				vector<Mxf_FileInfo>& mxfList = aPkl.mxfList;
				int cplCount = cplList.size();
				for ( int m = 0; m < cplCount; m++ )
				{
					bool bIsInvalid  = false;
					vector< string > mainItemList;//��¼UUID

					//���ｫ�Ƚ���Ƶ����Ƶ�ļ�uuidȡ�����뵽mainItemList�У�����mxfList�Ƚ���ɸѡ���ҳ���Ӧmxf�ļ���Ϣ
					CplInfo& cpl = cplList[m];	
					for ( vector< string >::size_type i = 0; i < cpl.mainPicList.size(); i++ )
						mainItemList.push_back( cpl.mainPicList[i] );

					for ( vector< string >::size_type i = 0; i < cpl.mainSoundList.size(); i++ )
						mainItemList.push_back( cpl.mainSoundList[i] );

					int nListCount = mainItemList.size();
					for ( int i = 0; i < nListCount; i++ )
					{
						string Uuid = mainItemList[i];
						int mxfCount = mxfList.size();
						for ( int n = 0; n < mxfCount; n++ )
						{
							Mxf_FileInfo& aMxfInfo = mxfList[n];
							//if ( "urn:uuid:" + Uuid == aMxfInfo.mxfUuid )
							if ( Uuid == aMxfInfo.mxfUuid )	//��mxfList�Ƚ���ɸѡ���ҳ���Ӧmxf�ļ���Ϣ
							{
								string mxfName = aMxfInfo.fileName;
								string fileName = mxfName;
								//check file exist 
								vector<cftp::FileInformation> fileList;
								ret = listFilesOnFTP( (cftp::FtpClient&)ftp, fileName.substr(0,fileName.rfind('/')+1), fileList, fileName.substr(fileName.rfind('/')+1) );
								if( ret != cftp::NOANYERROR )
									return ret;
								ret = ( fileList.size()!=0 && fileList[0].name==fileName );
								if( ret  )
								{
#ifdef _TEST_
									printf( "File  exists :%s\n", fileName.c_str() );
#endif
									long long nFileLength = atoll(fileList[0].size.c_str());
									if ( nFileLength == aMxfInfo.length )
									{
										bIsInvalid = false;
									}
									else
									{
										bIsInvalid = true;
										break;
									}
								}
								else
								{
#ifdef _TEST_
									printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
									bIsInvalid = true;
									break;
								}
								break;
							}
						}
						//���ֳ������⣬������ֹ��顣
						if ( bIsInvalid == true )
						{
							break;
						}
					}

					cpl.status = bIsInvalid;
				}
			}
		}
	}

	return 0;
}
int GetKdmInfo( const std::string &path, std::vector<KdmInfo> &kdmList, std::string& error )
{
	int ret(0);
	std::string Path;
	DcpInfo dcp;
	bool bIsFileName = false;
	if ( path.rfind(DIR_SEPCHAR) != (path.size()-1) )
	{
		bIsFileName = true;
	}

	if ( bIsFileName )
	{
		KdmInfo kdmInfo;
		ret = dcp.GetKdmInfo( path , kdmInfo , error );
		if( ret!=0 )
		{
			printf("GetKdmInfo error!\n");
#ifdef _TEST_
			error = "GetKdmInfo error!\n" + error;
#else
			error = "����δ�ҵ���Ч��KDM��";
#endif
			ret = CMTT(ERROR_CONTENT_KDM_GETKDMINFOERROR);
			WELOG(error)
		}
		else
		{
			kdmList.push_back(kdmInfo);
		}
		return ret;
	}
	else
	{
		Path = path;
		//Path[Path.length()-1] = '\0';
		Path.erase(Path.length()-1);
	}
	
	ret = dcp.GetKdmInfo( Path ,kdmList , error );
	if( ret!=0 )
	{
		printf("GetKdmInfo error!\n");
#ifdef _TEST_
		error = "GetKdmInfo error!\n" + error;
#else
		error = "����δ�ҵ���Ч��KDM��";
#endif
		ret = CMTT(ERROR_CONTENT_KDM_GETKDMINFOERROR);
		WELOG(error)
	}
	
	return ret;
}

int DeleteKdm( const std::string &path  ,std::string& error )
{
	int ret(0);
	if ( path.length()==0 )
	{
		error = "DeleteKdm() error!:bad Path";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_KDM_DELETEKDM_ERROR);
	}
	if( ( ret = access( path.c_str() , F_OK ) ) == 0 )     //check file exist
	{
#ifdef _TEST_
		printf( "File already exists :%s\n", path.c_str() );
		printf( "Delete file :%s\n", path.c_str() );
#endif
		remove( path.c_str() );
	}
	return ret;
}

int ImportKdm( std::vector<KdmInfo> &kdmList, const std::string &dest ,std::string& error )
{
	int ret(0);
	int nKdmInfoCount = kdmList.size();
	if (nKdmInfoCount==0)
	{
#ifdef _TEST_
		printf("error:KdmInfoCount is NULL !\n");
#endif
		error = "error:KdmInfoCount is NULL !\n";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_KDM_KDMINFOCOUNTISNULL_ERROR);
	}

     //check dest file exist
	if( ( ret = access( dest.c_str() , F_OK ) ) == 0 )
	{
#ifdef _TEST_
		printf( "File already exists :%s\n", dest.c_str() );
#endif
	}
	else
	{
#ifdef	WIN32
		//ret = mkdir( dest.c_str() );
		ret = system( (MKDIR + dest).c_str() );
#else
		//ret = mkdir( dest.c_str() ,0644 );
		ret = system( (MKDIR + dest).c_str() );
#endif

	}
	//copy kdm
	for ( int i=0; i< nKdmInfoCount; i++ )
	{
		KdmInfo& kdmInfo = kdmList[i];
		string path = kdmInfo.path;
		string srcFileName = path + kdmInfo.filename;
		string destFileName = dest + kdmInfo.filename;
		ret = cp_file( srcFileName.c_str(), destFileName.c_str() );
		kdmInfo.status = ret;		//0--success,��0--failed
	}
	return 0;
}
int DownLoadKdm( vector<KdmInfo> &kdmList, const string &dest , string& error )
{
	int ret(0);
	int nKdmInfoCount = kdmList.size();
	if (nKdmInfoCount==0)
	{
#ifdef _TEST_
		printf("error:KdmInfoCount is NULL !\n");
#endif
		error = "error:KdmInfoCount is NULL !\n";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_KDM_KDMINFOCOUNTISNULL_ERROR);
	}

	//check dest file exist
	if( ( ret = access( dest.c_str() , F_OK ) ) == 0 )
	{
#ifdef _TEST_
		printf( "File already exists :%s\n", dest.c_str() );
#endif
	}
	else
	{
#ifdef	WIN32
		//ret = mkdir( dest.c_str() );
		ret = system( (MKDIR + dest).c_str() );
#else
		//ret = mkdir( dest.c_str() ,0644 );
		ret = system( (MKDIR + dest).c_str() );
#endif

	}

	for ( int i=0; i< nKdmInfoCount; i++ )
	{
		KdmInfo kdmInfo = kdmList[i];
		string ftpUrl = kdmInfo.path;
		string srcFileName = kdmInfo.filename;
		//string sFileUrl = ftpUrl + kdmInfo.filename;
		//string destFileName = dest + kdmInfo.filename;
		cftp::FtpClient ftp;
		//log in
		int result = ftp.LogIn(ftpUrl);
		if(result != cftp::NOANYERROR)
			return CMTT2(result);

		string url = ftpUrl;
		
		string ip, userName, passwd, dir;
		string port = "21";

		result = ftp.ParseUrl(url, ip, userName, passwd, port, dir);
		if(result != cftp::NOANYERROR)
			return CMTT2(result);
		
		if ( srcFileName.rfind( '/' )!= string::npos )
		{
			srcFileName = srcFileName.substr( srcFileName.rfind( '/' )+1 );
		}

		ret = ftp.TransportFile( dir  , dest , srcFileName );
		if(ret != cftp::NOANYERROR)
			return CMTT2(ret);

		//@author zhangmiao@oristartech.com
		//@date [2014-07-18]
		//@brief ��ҪFTP����ʱЭ������(quit)��Ӧ������Ϣ���ȶ�������
		//@old ��ǰ����
		/*ret = ftp.LogOut();
		if(ret != cftp::NOANYERROR)
			return CMTT2(ret);*/
		//@new
		ftp.LogOut();
		//@modify end;
	}

	return 0;
}

int DeleteDcp( const std::string &path, const std::string &pklUuid , std::string& error )
{
	int ret(0);
	string sFilmFilePath = path;
	if(sFilmFilePath.rfind(DIR_SEPCHAR)!=sFilmFilePath.length()-1) 
		sFilmFilePath.append( 1, DIR_SEPCHAR );

	size_t pos;
	if ( ( pos = pklUuid.find("urn:uuid:") ) != pklUuid.npos && pos==0 )
	{
		sFilmFilePath = sFilmFilePath + pklUuid.substr(9);
	}
	else
	{
		sFilmFilePath = sFilmFilePath + pklUuid;
	}
	
	if ( sFilmFilePath.length()==0 )
	{
		error = "Param Path is Null!";
		WELOG(error)
		errno = ERROR_CONTENT_DCP_PARAM_PATH_IS_NULL;
		return CMTT2(errno);
	}

	if( ( ret = access( sFilmFilePath.c_str() , F_OK ) ) == 0 )     //check file exist
	{
#ifdef _TEST_
		printf( "File Directory already exists :%s\n", sFilmFilePath.c_str() );
#endif

		ret = PreRemoveDir(sFilmFilePath);
		if (ret == 0 )
		{
			printf( "\n Call PreRemoveDir() finished:%s\n", sFilmFilePath.c_str() );
		}
		else
		{
			printf( "\n Call PreRemoveDir() Failed:%s\n", sFilmFilePath.c_str() );
		}

		ret = system( (RMDIR + sFilmFilePath).c_str() );
		if( ret == 0 )
		{
#ifdef _TEST_
			printf( "\nDirectory deleted :%s\n", sFilmFilePath.c_str() );
#endif

			//DeleteDcp ����д��־
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"\nDeleteDcp()---Directory deleted :%s\n", sFilmFilePath.c_str() );
			string logbuffer;
			logbuffer = buffer;
			WELOG(logbuffer);

			errno = 0;
			return 0;
		}
		else
		{
#ifdef _TEST_
			perror("\nUnable to delete directory\n");
#endif
			error = "\nUnable to delete directory\n";

			WELOG(error)
			return CMTT2(ERROR_CONTENT_DCP_UNABLE_TO_DELETE_DIR);
		}
	}
	else
	{
		//Do not found pkl
		error = "Do not found pkl";
		WELOG(error)
		errno = ERROR_CONTENT_DCP_DONOT_FOUND_PKL;
		return CMTT2(ERROR_CONTENT_DCP_DONOT_FOUND_PKL);
	}

	return ret;
}

int HashDcp( const std::string &path, const std::string &pklUuid ,std::string& error )
{
	bool errorOccurred = false;
	bool bIsFoundPKL   = false;
	int ret(0);
	DcpInfo dcp;
	vector< AMInfo > aAMInfo_Vt;
	ret = dcp.CreateDCPInfo( path , error );
	if( ret!=0 )
	{
		error = "error:CreateDCPInfo()!\n" + error;
		printf("error:CreateDCPInfo()!\n");
		WELOG(error)
		ret = CMTT(ERROR_CONTENT_DCP_CREATE_DCPINFO_ERROR);
	}

	if ( ret==0 )
	{
		aAMInfo_Vt = dcp.vAMInfo_t;
		int nAMInfoCount = aAMInfo_Vt.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AMInfo_t aAMInfo = aAMInfo_Vt[i];
			//-------------------
			vector< dcp_Info_t > vDcp = aAMInfo.vDcp;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				dcp_Info_t dcp = vDcp[j];
				pkl_Info_t aPkl = dcp.aPkl;	
				if ( pklUuid == aPkl.uuid )
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif
					g_sHashDcp_PklUuidName = pklUuid;
					g_AllFile_Size = aPkl.allFileLength;
					g_hasHashChecked_Size = 0ull;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif

					//����bStopSha1CheckFlag��Ϊ����ֵ�����ڷ���״ֵ̬��hashУ���Ƿ�ȡ��;
					//���hashУ�鱻ȡ�����򷵻�ֵ����=1�����򷵻�ֵ����=0��
					int bStopSha1CheckFlag = 0;

					bIsFoundPKL = true;
					vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
						string hashValue = aAsset_Pkl.hash;
						string fileName = aAsset_Pkl.originalFileName;
     
						//�ȼ���ļ��Ƿ���ڣ�check file exist
						if( ( ret = access( fileName.c_str() , F_OK ) ) == 0 )
						{
#ifdef _TEST_
							printf( "File  exists :%s\n", fileName.c_str() );
#endif
						}
						else  //file NOT exist
						{
							char buffer[BUF_SIZE]="";
							sprintf( buffer,"%s:%s\n", fileName.c_str(), strerror(errno) );
							error = buffer;
#ifdef _TEST_
							printf( "File NOT exists :%s\n", fileName.c_str() );

							printf( "%s:%s\n", fileName.c_str(),strerror(errno) );
#endif
							errno = ERROR_CONTENT_DCP_FILE_NOT_EXIST;
							WELOG(error)
							return errno;
						}


						//making hash check begin
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sHashDcp_Checking_FileName = fileName;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif

						HashInfo sha1;
						char bin_buffer[SHA1_DIGEST_SIZE] = {0};
						//string hashStr = sha1.getSha1FromFile(fileName.c_str(),bin_buffer,NULL,NULL,0);
						string hashStr = sha1.getSha1FromFile(fileName.c_str(),bin_buffer,NULL, &bStopSha1CheckFlag ,0);
						if(hashStr.empty())
						{
							if ( bStopSha1CheckFlag == 1 )
							{
								//CODE_SHA1CHECKCANCEL
								error = "ERROR:getSha1FromFile is Canceled!\n SHA1 Check Is Canceled!\n";
								cout << "getSha1FromFile is Canceled! SHA1 Check Is Canceled!" << endl;
							}
							else
							{
								error = "ERROR:getSha1FromFile is failed!";
								cout << "getSha1FromFile is failed!" << endl;
							}
							
							errorOccurred = true;
							break;
						}
						std::string b64Str = sha1.toBase64(bin_buffer);
						if(b64Str.empty())
						{
							error = "ERROR:toBase64 is failed!";
							cout << "toBase64 is failed!" << endl;
							errorOccurred = true;
							break;
						}
						if(0!=hashValue.compare(b64Str))
						{
							char buffer[BUF_SIZE]="";
							sprintf( buffer,"hash check is failed! [fileName=%s],hash value:[%s , %s]\n", fileName.c_str() , hashValue.c_str(),b64Str.c_str() );
							error = buffer;

							printf("hash check is failed! [fileName=%s],hash value:[%s , %s]\n", fileName.c_str() , hashValue.c_str(),b64Str.c_str());
							errorOccurred = true;
							break;
						}
						//making hash check end
					}
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif
					if( errorOccurred )
					{
						//hashУ�鱻ȡ��
						if ( bStopSha1CheckFlag == 1 )
						{
							errno = CODE_SHA1CHECKCANCEL;
							g_sHashDcp_PklUuidName = "";
							g_AllFile_Size = 0ull;
#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							WELOG(error)
							return CMTT2(ERROR_CONTENT_HASH_SHA1_CHECK_CANCEL);
						}
						
						errno = CODE_SHA1CHECK_ERROR;
						g_sHashDcp_PklUuidName="";
						g_AllFile_Size = 0ull;
#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						error = "HashDcp(),SHA1 CHECK ERROR:[PklUuid=" + pklUuid + "]!" + error;
						//return 1;
						WELOG(error)
						return CMTT2(ERROR_CONTENT_HASH_SHA1_CHECK_ERROR);
					}
					if ( bIsFoundPKL && (errorOccurred==false) )
					{
						errno = 0;
						g_sHashDcp_PklUuidName="";
						g_sHashDcp_Checking_FileName="";
						//g_AllFile_Size = 0ull;
#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif

						error = "HashDcp(),SHA1 CHECK Sucess:[PklUuid=" + pklUuid + "]!" + error;
						
						WELOG(error)
						return 0;
					}
				}
				
			}
			
		}
		//Do not found pkl
		if (bIsFoundPKL==false)
		{
#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif
			g_sHashDcp_PklUuidName="";
			g_sHashDcp_Checking_FileName="";
			g_hasHashChecked_Size = g_AllFile_Size = 0ull;
#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			
			error = "HashDcp(),Do not found PKL:[PklUuid=" + pklUuid + "]!";
			//return -1;
			WELOG(error)
			errno = ERROR_CONTENT_DCP_DONOT_FOUND_PKL;
			return CMTT2(ERROR_CONTENT_DCP_DONOT_FOUND_PKL);
		}
	}

#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	g_sHashDcp_PklUuidName="";
	g_AllFile_Size = 0ull;

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return ret;
}

int GetHashPercent( const std::string &path, const std::string &pklUuid, Hashinfo &hash ,std::string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	int nPercent = 0;
	if ( g_sHashDcp_PklUuidName == pklUuid )
	{
		long double  fRate = ((long double)(g_hasHashChecked_Size)) / ((long double)(g_AllFile_Size));
		nPercent = int(fRate*100.0);
		if (nPercent==100)
		{
			hash.status = STATUS_FINISHED;
		}
		else
		{
			hash.status = STATUS_CHECKING;
		}
		
		if ( errno == CODE_SHA1CHECK_ERROR )
		{
			error = "error:DCP Hash Check is STATUS_ERROR.";
			hash.filename = g_sHashDcp_Checking_FileName;
			hash.status = STATUS_ERROR;
			WELOG(error)
		}

		hash.percent = nPercent;
		
		hash.path = path;
		
	}
	else
	{
		hash.status = STATUS_NOSTART;
		hash.percent = 0;
		hash.path = path;

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

		error = "error:DCP Hash Check is NO START.";
		WELOG(error)
		return CMTT2(ERROR_CONTENT_HASH_HASHCHECK_NO_START);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}

int CancelHash( const std::string &path, const std::string &pklUuid ,std::string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	if ( g_sHashDcp_PklUuidName == pklUuid )
	{
		if ( g_flag_SHA1CheckStop == 0 )
		{
			g_flag_SHA1CheckStop = 1;
		}
		else if ( g_flag_SHA1CheckStop == 1 && errno == CODE_SHA1CHECKCANCEL )
		{
			g_flag_SHA1CheckStop = 0;
			g_sHashDcp_PklUuidName = "";
		}
	}
	else
	{
		g_sHashDcp_PklUuidName = "";

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

		error = "PklUuid is error.";
		WELOG(error)
		return CMTT2(ERROR_CONTENT_DCP_PKL_UUID_ERROR);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}

int cp_file_BP( const char *src, const char *des , unsigned long long& rlTransferredSize , unsigned long long from )
{
	//src can not be same as the des;
	int in,out,n=0,nW=0;
	unsigned long long nTransferredSize = 0ull;
	char buf[BSIZE]="";

	if(src==NULL || des==NULL) {printf("para is NULL!\n");return -1;}
	if(strcmp(src,des)==0){printf("src is the same as des!\n");return -1;}
	if((in=open(src,O_RDONLY|O_BINARY))==-1)
	{
#ifdef _TEST_
		printf("cp_f:fail to open %s:%s\n",src,strerror(errno));
#endif
		return -1;
	}
	if((out=open(des,O_WRONLY|O_CREAT|O_BINARY,0644))==-1)
	{
#ifdef _TEST_
		printf("cp_f:fail to open %s:%s\n",des,strerror(errno));
#endif
		close(in);
		return -1;
	}
	//printf("in=%d out=%d\n",in,out);

	//------------------------
	if (from<0)
	{
#ifdef _TEST_
		printf( "Error:File start position is %llu\n",from );
#endif
		return -1;
	}

	unsigned long long nlPos = lseek(in, (long)from, SEEK_SET);
	if (nlPos<0)
	{
#ifdef _TEST_
		printf( "Error:File start position is %llu\n",nlPos );
#endif
		return -1;
	}
	nlPos = lseek(out, (long)from, SEEK_SET);
	if (nlPos<0)
	{
#ifdef _TEST_
		printf( "Error:File start position is %llu\n",nlPos );
#endif
		return -1;
	}
	nTransferredSize += from;
	//--------------------

	while( ( n = read( in,buf,BSIZE ) ) >0 )
	{
		nW = 0;
		if( ( nW = write(out,buf,n) ) != n )
		{
			close(out);close(in);
#ifdef _TEST_
			printf("file handle %d and %d is closed\n",in,out);
#endif
			return -1;
		}
		memset( buf, 0, BSIZE );

#ifdef WIN32
		WaitForSingleObject(hMutex,INFINITE);
#else
		pthread_mutex_lock(&work_mutex);
#endif
		g_hasDcpCopied_Size += nW;
		nTransferredSize += nW;

		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		//FileCopyInfo& copyInfo   = g_map_FileCopyInfo[string(des)]; 
		FileCopyInfo& copyInfo   = g_map_FileCopyInfo[string(src)];
		//@modify end;
		copyInfo.transferredSize = int(nTransferredSize/ONEMB);

		if( g_flag_DcpCopyStop == 1 )
		{
			close(out);
			close(in);
			rlTransferredSize = nTransferredSize;

#ifdef _TEST_
			printf("copy DCP received the stop signal! Code is %d\n",CODE_DCPCOPYCANCEL);
#endif
#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif

			errno = CODE_DCPCOPYCANCEL;
			return CODE_DCPCOPYCANCEL;
		}

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

	}
	rlTransferredSize = nTransferredSize;
	close(out);close(in);
#ifdef _TEST_
	printf("file handle %d and %d is closed\n",in,out);
#endif
	if(n<0) return -1;
	return 0;
}

int cp_file_Mode2( const char *src, const char *des )
{
	//src can not be same as the des;
	int in,out,n=0,nW=0;
	unsigned long long nTransferredSize = 0ull;
	char buf[BSIZE]="";

	if(src==NULL || des==NULL) {printf("para is NULL!\n");return -1;}
	if(strcmp(src,des)==0){printf("src is the same as des!\n");return -1;}
	if((in=open(src,O_RDONLY|O_BINARY))==-1)
	{
#ifdef _TEST_
		printf("cp_f:fail to open %s:%s\n",src,strerror(errno));
#endif
		return -1;
	}
	if((out=open(des,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,0644))==-1)
	{
#ifdef _TEST_
		printf("cp_f:fail to open %s:%s\n",des,strerror(errno));
#endif
		close(in);
		return -1;
	}
	//printf("in=%d out=%d\n",in,out);
	while( ( n = read( in,buf,BSIZE ) ) >0 )
	{
		nW = 0;
		if( ( nW = write(out,buf,n) ) != n )
		{
			close(out);close(in);
#ifdef _TEST_
			printf("file handle %d and %d is closed\n",in,out);
#endif
			return -1;
		}
		memset( buf, 0, BSIZE );

#ifdef WIN32
		WaitForSingleObject(hMutex,INFINITE);
#else
		pthread_mutex_lock(&work_mutex);
#endif
		g_hasDcpCopied_Size += nW;
		nTransferredSize += nW;

		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		//FileCopyInfo& copyInfo   = g_map_FileCopyInfo[string(des)];
		FileCopyInfo& copyInfo   = g_map_FileCopyInfo[ string(src) ];
		//@modify end;
		copyInfo.transferredSize = int(nTransferredSize/ONEMB);

		if( g_flag_DcpCopyStop == 1 )
		{
			close(out);
			close(in);
#ifdef _TEST_
			printf("copy DCP received the stop signal! Code is %d\n",CODE_DCPCOPYCANCEL);
#endif

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif

			errno = CODE_DCPCOPYCANCEL;
			return CODE_DCPCOPYCANCEL;
		}

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

	}
	close(out);close(in);
#ifdef _TEST_
	printf("file handle %d and %d is closed\n",in,out);
#endif
	if(n<0) return -1;
	return 0;
}
int cp_file(const char *src, const char *des)
{
	//src can not be same as the des;
	int in,out,n;
	char buf[BSIZE];

	if(src==NULL || des==NULL) {printf("para is NULL!\n");return -1;}
	if(strcmp(src,des)==0){printf("src is the same as des!\n");return -1;}
	if((in=open(src,O_RDONLY|O_BINARY))==-1){
		printf("cp_f:fail to open %s:%s\n",src,strerror(errno));
		return -1;
	}
	if((out=open(des,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,0644))==-1){
		printf("cp_f:fail to open %s:%s\n",des,strerror(errno));
		close(in);
		return -1;
	}
	//printf("in=%d out=%d\n",in,out);
	while(0<(n=read(in,buf,BSIZE))){
		if(n!=write(out,buf,n))
		{
			close(out);close(in);//printf("%d and %d is closed\n",in,out);
			return -1;
		}
		/*if (eof (in))
			break;*/
	}
	close(out);close(in);//printf("%d and %d is closed\n",in,out);
	if(n<0) return -1;
	return 0;
}
//ȡ��DCP����
int CancelCopy( const string &source, const string &pklUuid, string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	if (g_sCopyDcp_PklUuidName==pklUuid)
	{
		if ( g_flag_DcpCopyStop == 0 )
		{
			g_flag_DcpCopyStop = 1;
		}
		else if ( g_flag_DcpCopyStop == 1 && errno==CODE_DCPCOPYCANCEL )
		{
			g_flag_DcpCopyStop = 0;
			g_sCopyDcp_PklUuidName="";
		}
	}
	else
	{
		g_sCopyDcp_PklUuidName="";

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif
		error = "PklUuid is error.";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_DCP_PKL_UUID_ERROR);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}
//ȡ��DCP�������
int GetCopyPercent( const string &source , const string &pklUuid, CopyInfo &info, string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	int nPercent = 0;
	if ( g_sCopyDcp_PklUuidName == pklUuid )
	{
		long double  fRate = ((long double)(g_hasDcpCopied_Size)) / ((long double)(g_Dcp_AllFile_Size));
		nPercent = int(fRate*100.0);
		if (nPercent==100)
		{
			info.status = DCP_STATUS_FINISHED;
		}
		else
		{
			info.status = DCP_STATUS_COPING;
		}

		if ( errno != 0 )
		{
			error = "error:DCP Copy Percent is DCP_STATUS_ERROR.";
			info.status = DCP_STATUS_ERROR;
			WELOG(error)
		}

		info.percent = nPercent;

		info.dest = "";
		info.source = source;

		if (g_map_FileCopyInfo.size())
		{
			info.copyInfo.clear();
			map< std::string , FileCopyInfo >::iterator iFirst = g_map_FileCopyInfo.begin();
			map< std::string , FileCopyInfo >::iterator iLast = g_map_FileCopyInfo.end();
			for (; iFirst != iLast; ++iFirst)
				info.copyInfo.push_back((*iFirst).second);
		}
	}
	else
	{
		info.status = DCP_STATUS_NOSTART;
		info.percent = 0;
		info.source = source;
		info.copyInfo.clear();

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

		error = "error:DCP Copy is NO START.";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_DCP_COPY_NO_START);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}

//�ϵ�������ʽ������copyInfo�е�ÿ���ļ�������CPL�ļ�ֱ�ӿ���
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int CopyDcp_BPTransSub1( const std::string &Dest, Content::Dcp::pkl_Info_t& aPkl, std::vector<Content::FileCopyInfo> &copyInfo , std::string& error )
int CopyDcp_BPTransSub1( const std::string &source, const std::string &Dest, pkl_Info_t& aPkl, std::vector<FileCopyInfo> &copyInfo , std::string& error )
//@modify end;
{
	int ret(0);
	int nSize_Copy_Info = copyInfo.size();
	for( int s=0; s<nSize_Copy_Info; s++ )
	{
		FileCopyInfo& rFileCopyInfo = copyInfo[s];

		if (rFileCopyInfo.bIsFinished==1)		//���������ɣ��������һ���ļ���
		{
			//----zhangmiao:begin--[7/20/2012]----
			long long Length = 0;
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//@new
			//ret = GetFileSize_PKLAssetList( rFileCopyInfo.filename , aPkl, Length );
			string sFullPathFileName;
			ret = GetFileSize_PKLAssetList( rFileCopyInfo.filename , aPkl, Length , sFullPathFileName );
			if (ret==0)
			{
				g_hasDcpCopied_Size += Length;
			}
			//----zhangmiao:end----[7/20/2012]----
			
			//���»�ȡ����ʱ��״̬
			//----zhangmiao:begin--20121226----
#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//@new
			/*string destFileName = Dest + rFileCopyInfo.filename;
			map< string , FileCopyInfo >::size_type nCont = g_map_FileCopyInfo.count(destFileName);
			if ( nCont == 1 )
			{
				FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
				copyInfo.bIsFinished = 1;
			}*/

			string srcFileName = sFullPathFileName;
			map< string , FileCopyInfo >::size_type nCont = g_map_FileCopyInfo.count(srcFileName);
			if ( nCont == 1 )
			{
				FileCopyInfo& copyInfo   = g_map_FileCopyInfo[srcFileName]; 
				copyInfo.bIsFinished = 1;
			}
			//@modify end;
#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			//----zhangmiao:end----20121226----
			
			continue;
		}

		string sFileCopyName = rFileCopyInfo.filename;
		if ( sFileCopyName.length()==0 )
		{
			error = "Error:FileCopyName is NULL!\n";
			//errno = -2;
			errno = ERROR_CONTENT_DCP_FILECOPYNAME_IS_NULL;
			WELOG(error)
			return errno;
		}

		//Copy PKL AssetList File
		vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
		int nSize_Asset_Pkl = vAsset_pkl.size();
		for ( int k=0; k< nSize_Asset_Pkl; k++ )
		{
			asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
			string fileType = aAsset_Pkl.type;
			string fileName = aAsset_Pkl.originalFileName;
			if ( fileName.length()==0 )
			{
				error = "Error:Pkl AssetFile Path Is NULL!\n";
				//errno = -2;
				WELOG(error)
				errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
				return errno;
			}

			if ( sFileCopyName == fileName.substr(fileName.rfind(DIR_SEPCHAR)+1) )    //�ж��ļ��Ƿ��ҵ�
			{
				if ( fileType == string("text/xml;asdcpKind=CPL") )      //ֱ�ӿ���CPL�ļ�
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					if( ( ret = access( fileName.c_str() , F_OK ) ) == 0 )     //check file exist
					{
#ifdef _TEST_
						printf( "File  exists :%s\n", fileName.c_str() );
#endif
						//printf( "Delete file :%s\n", fileName.c_str() );
						//ret = remove( fileName.c_str() );

						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
/*
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);
						aCopyInfo.transferredSize = 0;
						g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1)]=aCopyInfo;
#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
*/
						//@modify end;

						string srcFileName = fileName;
						string destFileName = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);

						ret = cp_file_Mode2( srcFileName.c_str(), destFileName.c_str() );

						if (ret==0)
						{
							//----zhangmiao:begin--20121226----
							//���»�ȡ����ʱ��״̬��copy CPL file
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif
							
							//@author zhangmiao@oristartech.com
							//@date [2013-07-15]
							//��Ҫ�����������Ļ�ļ��Ĵ������
							//@new
							//FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName];
							FileCopyInfo& copyInfo   = g_map_FileCopyInfo[srcFileName];
							//@modify end;

							copyInfo.bIsFinished = 1;

#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							//----zhangmiao:end----20121226----

							rFileCopyInfo.bIsFinished = 1;
#ifdef _TEST_
							printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
						}
						else
						{
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif

							g_sCopyDcp_PklUuidName="";
							g_Dcp_AllFile_Size = 0ull;

							if ( ret == CODE_DCPCOPYCANCEL )
							{
								g_flag_DcpCopyStop = 0;

								char buffer[BUF_SIZE]="";
								sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
								error = buffer;
#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

								errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
							}
							else
							{
#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
								errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
							}

#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							//errno = 1;
							WELOG(error)
							return errno;
						}
					}
					else  //file NOT exist
					{
						char buffer[BUF_SIZE]="";
						sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
						error = buffer;
#ifdef _TEST_
						printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName = "";
						g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						errno = ERROR_CONTENT_DCP_CPL_FILE_IS_NO_EXIST;
						WELOG(error)
						return errno;
					}
				}
				//@author zhangmiao@oristartech.com
				//@date [2013-07-15]
				//@new
				//��Ҫ�����������Ļ�ļ��Ĵ������
				else if ( fileType == string("text/xml;asdcpKind=Subtitle") || fileType == string("image/png") || fileType == string("application/ttf") )
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif

					if( ( ret = access( fileName.c_str() , F_OK ) ) == 0 )     //check file exist
					{
#ifdef _TEST_
						printf( "File  exists :%s\n", fileName.c_str() );
#endif

						//���Ŀ��Ŀ¼������Ŀ¼��������Ӧ����Ŀ¼��
						string tmpPath,SubPath;
						size_t pos1 = fileName.rfind( source );
						if ( pos1 != string::npos )
						{
							tmpPath = fileName.substr( pos1 + source.length() );
						}
						size_t pos2 = tmpPath.rfind(DIR_SEPCHAR);
						if ( pos2 != string::npos )
						{
							SubPath = tmpPath.substr( 0, pos2+1 );
						}

						string DestFile_InPath = Dest + SubPath;
						ret = MakeDirectory2(DestFile_InPath);		//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
						if(ret!=0)
						{
							error = "Error:CopyDcp_BPTransSub1--Make Director2 fail.\n";
							WELOG(error);
							ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
							return ret;
						}
	
						string srcFileName  = fileName;
						string destFileName = DestFile_InPath + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);

						ret = cp_file_Mode2( srcFileName.c_str(), destFileName.c_str() );

						if ( ret == 0 )
						{
							//���»�ȡ����ʱ��״̬��copy file
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif

							FileCopyInfo& copyInfo   = g_map_FileCopyInfo[srcFileName];
							copyInfo.bIsFinished = 1;

#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							

							rFileCopyInfo.bIsFinished = 1;
#ifdef _TEST_
							printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
						}
						else
						{
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif

							g_sCopyDcp_PklUuidName="";
							g_Dcp_AllFile_Size = 0ull;

							if ( ret == CODE_DCPCOPYCANCEL )
							{
								g_flag_DcpCopyStop = 0;

								char buffer[BUF_SIZE]="";
								sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
								error = buffer;
#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

								errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
							}
							else
							{
#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
								errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
							}

#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							//errno = 1;
							WELOG(error);
							return errno;
						}
					}
					else  //file NOT exist
					{
						char buffer[BUF_SIZE]="";
						sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
						error = buffer;
#ifdef _TEST_
						printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName = "";
						g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						errno = ERROR_CONTENT_DCP_FILE_NOT_EXIST;
						WELOG(error);
						return errno;
					}
				}
				//@modify end;
				else  //copy mxf file
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					if( ( ret = access( fileName.c_str() , F_OK ) ) == 0 )     //check file exist
					{
#ifdef _TEST_
						printf( "File  exists :%s\n", fileName.c_str() );
#endif
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
						/*FileCopyInfo aCopyInfo;
						aCopyInfo.filename = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);
						aCopyInfo.transferredSize = 0;
						g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1)]=aCopyInfo;*/
						//@modify end;

						string srcFileName = fileName;
						string destFileName = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);

						unsigned long long nlTransferredSize = 0;
						unsigned long long nfrom = 0;
						nlTransferredSize = rFileCopyInfo.transferredSize*ONEMB;
						nfrom = nlTransferredSize;
						//----zhangmiao:begin--[7/20/2012]----
						g_hasDcpCopied_Size += nlTransferredSize;
						//----zhangmiao:end----[7/20/2012]----
#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif

						ret = cp_file_BP(  srcFileName.c_str(), destFileName.c_str() , nlTransferredSize, nfrom );
						rFileCopyInfo.transferredSize = static_cast<int>(nlTransferredSize/ONEMB);
						rFileCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);

						if ( ret == 0 )
						{
							//----zhangmiao:begin--20121226----
							//���»�ȡ����ʱ��״̬��copy mxf file
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif

							//@author zhangmiao@oristartech.com
							//@date [2013-07-15]
							//��Ҫ�����������Ļ�ļ��Ĵ������
							//@new
							//FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
							FileCopyInfo& copyInfo   = g_map_FileCopyInfo[srcFileName];
							//@modify end;
							copyInfo.bIsFinished = 1;

#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							//----zhangmiao:end----20121226----

							rFileCopyInfo.bIsFinished = 1;
#ifdef _TEST_
							printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
						}
						else 
						{
#ifdef WIN32
							WaitForSingleObject(hMutex,INFINITE);
#else
							pthread_mutex_lock(&work_mutex);
#endif

							g_sCopyDcp_PklUuidName = "";
							g_Dcp_AllFile_Size = 0ull;

							if ( ret == CODE_DCPCOPYCANCEL )
							{
								g_flag_DcpCopyStop = 0;
								
								char buffer[BUF_SIZE]="";
								sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy BPTrans CANCEL!" );
								error = buffer;

#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),"DCP Copy BPTrans CANCEL!");
#endif

								errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
							}
							else
							{
#ifdef _TEST_
								printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
								errno = ERROR_CONTENT_DCP_MXF_FILE_COPY_IS_UNFINISHED;
							}


#ifdef WIN32
							ReleaseMutex(hMutex);
#else
							pthread_mutex_unlock(&work_mutex);
#endif
							//errno = 1;
							WELOG(error)
							return errno;
						}
					}
					else  //file NOT exist
					{
						char buffer[BUF_SIZE]="";
						sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
						error = buffer;
#ifdef _TEST_
						printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName="";
						g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif

						errno = ERROR_CONTENT_DCP_MXF_FILE_IS_NO_EXIST;
						WELOG(error)
						return errno;
					}
				}
			}
			else
			{	//û���ҵ��������������һ���ļ�
				continue;
			}

		}

	}

	return ret;
}
//�ϵ�������ʽ������DCP����
int CopyDcp_BPTrans( const string &source, const string &pklUuid, const string &dest, vector<FileCopyInfo> &copyInfo , std::string& error )
{
	bool errorOccurred = false;
	bool bIsFoundPKL   = false;
	int ret(0);
	DcpInfo dcp;
	vector< AMInfo > aAMInfo_Vt;
	ret = dcp.CreateDCPInfo( source , error );	//������ȵ�source��ָĿ¼��ȥ���������Ƿ�������ӰƬ��sourceĿ¼�����趨Ϊ����ƶ�Ӳ�̹���Ŀ¼��
	if(ret!=0)
	{
#ifdef _TEST_
		printf("error CreateDCPInfo !\n");
#endif
		error = "error:CreateDCPInfo()!\n" + error;

		WELOG(error)
		ret = ERROR_CONTENT_DCP_CREATE_DCPINFO_ERROR;
		return ret;
	}
	//Parameters Check
	string sFilmFilePath = dest;
	if( sFilmFilePath.rfind(DIR_SEPCHAR) != sFilmFilePath.length()-1 )	//���destĿ¼��ĩβ�Ƿ���ָ��� "/" ��"\"�����������������ϡ�
		sFilmFilePath.append( 1, DIR_SEPCHAR );

	size_t pos;
	if ( ( pos = pklUuid.find("urn:uuid:") ) != pklUuid.npos && pos==0 )
	{
		sFilmFilePath = sFilmFilePath + pklUuid.substr(9);
	}
	else
	{
		sFilmFilePath = sFilmFilePath + pklUuid;
	}
	sFilmFilePath.append( 1, DIR_SEPCHAR );

	if ( sFilmFilePath.length()==0 )
	{
		error = "Error:FilmFilePath is NULL!\n";
		//errno = -2;
		WELOG(error)
		errno = ERROR_CONTENT_DCP_FILM_FILE_PATH_ISNULL;
		return errno;
	}

	string  Dest = sFilmFilePath;	//���յ�DestĿ¼Ϊ"destĿ¼" + '/' + "pklUuid" + "/"

	ret = MakeDirectory2(Dest);		//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
	if(ret!=0)
	{
		error = "Error:Make Directory fail.\n";
		WELOG(error)
		ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
		return ret;
	}
	else if ( ret==0 )	//dcp copy files
	{
		aAMInfo_Vt = dcp.vAMInfo_t;
		int nAMInfoCount = aAMInfo_Vt.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AMInfo_t aAMInfo = aAMInfo_Vt[i];
			//-------------------
			vector<asset_assetmap_t> aAssetVt = aAMInfo.aAssetmap_Info.asset;
			vector< dcp_Info_t > vDcp = aAMInfo.vDcp;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				string pklName;
				//����pklName�ļ�·����
				int nAssetVtCount = aAssetVt.size();
				for ( int k=0; k< nAssetVtCount; k++ )
				{
					if ( pklUuid == aAssetVt[k].uuid )
					{
						pklName =  aAssetVt[k].path;
						if (pklName.length()==0)	//�������Ϊ�գ���������
						{
							char buffer[BUF_SIZE]="";
							sprintf( buffer,"pklName %s Error!\n", pklName.c_str() );
							error = buffer;
#ifdef _TEST_
							printf("pklName %s Error!\n", pklName.c_str());
#endif
							//return -1;
							//ɾ�������ļ��У�Remove DCP Directory
							int nResult = RemoveDir( Dest );
							if( nResult != 0 )
							{
#ifdef _TEST_
								printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
								error += "Warning:Remove DCP Directory fail.\n";
							}
							WELOG(error)
							errno = ERROR_CONTENT_DCP_PKL_PKLNAME_ERROR;
							return errno;
						}

#ifdef _TEST_
						printf( "pklName:%s\n", pklName.c_str() );
#endif
						break;
					}
				}

				dcp_Info_t dcpInf = vDcp[j];
				pkl_Info_t aPkl = dcpInf.aPkl;	
				if ( pklUuid == aPkl.uuid )
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_PklUuidName = pklUuid;
					g_Dcp_AllFile_Size = aPkl.allFileLength;
					g_hasDcpCopied_Size = 0ull;

					bIsFoundPKL = true;        //pkl�ҵ�
					g_map_FileCopyInfo.clear();

					// 12/26/2012 zhangmiao begin 
					//��ʼ��g_map_FileCopyInfo ӳ��ṹ������pkl�������ļ�״̬��
					vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error)
						return errno;
					}
					for ( int m=0; m< nSize_Asset_Pkl; m++ )
					{
						asset_pkl_t aAsset_Pkl = vAsset_pkl[m];
						string fileName = aAsset_Pkl.originalFileName;
						if ( fileName.length() == 0 )
						{
							error = "Error:Pkl AssetFile Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error)
							return errno;
						}

						FileCopyInfo aCopyInfo;
						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
						//aCopyInfo.filename = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						aCopyInfo.filename = fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						//@modify end;
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);
						aCopyInfo.bIsFinished = 0;
						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
						//g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1)] = aCopyInfo;
						g_map_FileCopyInfo[ fileName ] = aCopyInfo;
						//@modify end;
					}
					// 12/26/2012 zhangmiao end 
#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					
					//------����copyInfo�е�ÿ���ļ�------
					//@author zhangmiao@oristartech.com
					//@date [2013-07-15]
					//��Ҫ�����������Ļ�ļ��Ĵ������
					//@new
					//ret = CopyDcp_BPTransSub1( Dest, aPkl, copyInfo , error );
					ret = CopyDcp_BPTransSub1( source , Dest, aPkl, copyInfo , error );
					//@modify end;
					if ( ret != 0 )
					{

#ifdef _BP_DCPCOPYCANCEL_
						//����ϵ�����ʱ��DCP������;��ȡ��������ʱĿǰ��ɾ��δ�������DCPĿ¼�����Ժ����ӡ���ͣ���ܡ�ʱ�ٲ�ɾ��δ�������DCPĿ¼��
						//����ԭ�������Ŀ����жϣ���ɾ�������ļ��У�
						//if ( ret != ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL )
						{
							//ɾ�������ļ��У�Remove DCP Directory
							int nResult = RemoveDir( Dest );
							if( nResult != 0 )
							{
#ifdef _TEST_
								printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif //_TEST_
								error += "Warning:Remove DCP Directory fail.\n";
							}
						}
#endif //_BP_DCPCOPYCANCEL_

						return ret;
					}
					//----����copyInfo�е�ÿ���ļ�:end--------

					//Copy PKL File
					ret = CopyDcpSub2( pklName, Dest, error );
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return ret;
					}

					//Copy AssetMap File
					string AssetmapName = aAMInfo.assetmapName;
					ret = CopyDcpSub3( AssetmapName, pklUuid, Dest, dcp, aPkl , error );
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return ret;
					}

					if ( bIsFoundPKL && (errorOccurred==false) )
					{
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						errno = 0;
						g_sCopyDcp_PklUuidName="";
						g_sCopyDcp_Copying_FileName="";

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						return 0;
					}
				}
			}
		}
		//Do not found pkl û���ҵ�PKLʱ������Ҫ�����⴦��
		if (bIsFoundPKL==false)
		{
			//ɾ�������ļ��У�Remove DCP Directory
			ret = RemoveDir(Dest);
			if( ret != 0 )
			{
#ifdef _TEST_
				printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
				error += "Warning:Remove DCP Directory Fail.\n";
			}

#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif

			g_sCopyDcp_PklUuidName="";
			g_sCopyDcp_Copying_FileName="";
			g_hasDcpCopied_Size = g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			error = "Do not found pkl\n";

			//return -1;
			WELOG(error)
			errno = ERROR_CONTENT_DCP_DONOT_FOUND_PKL;
			return errno;
		}
	}
	return 0;
}

//�ϵ�������ʽ������DCP����
int DownLoadDcp_BPTrans( const string &ftpUrl, const string &pklUuid, const string &dest,
						 vector<FileCopyInfo> &copyInfo, string& error )
{
	bool errorOccurred = false;
	bool bIsFoundPKL   = false;
	int ret(0);
	
	vector<AssetmapInfo> dcpList;
	ret = SearchFtpDcpList( ftpUrl, dcpList ,error );
	if(ret!=0)
	{
#ifdef _TEST_
		printf("error SearchFtpDcpList !\n");
#endif
		WELOG("error SearchFtpDcpList !\n")
		return ret;
	}

	//Parameters Check
	string sFilmFilePath = dest;
	if( sFilmFilePath.rfind(DIR_SEPCHAR) != sFilmFilePath.length()-1 )	//���destĿ¼��ĩβ�Ƿ���ָ��� "/" �����������������ϡ�
		sFilmFilePath.append( 1, DIR_SEPCHAR );

	size_t pos;
	if ( ( pos = pklUuid.find("urn:uuid:") ) != pklUuid.npos && pos==0 )
	{
		sFilmFilePath = sFilmFilePath + pklUuid.substr(9);
	}
	else
	{
		sFilmFilePath = sFilmFilePath + pklUuid;
	}
	sFilmFilePath.append( 1, DIR_SEPCHAR );

	if ( sFilmFilePath.length()==0 )
	{
		error = "Error:FilmFilePath is NULL!\n";
		//errno = -2;
		WELOG(error)
		errno = ERROR_CONTENT_DCP_FILM_FILE_PATH_ISNULL;
		return errno;
	}

	//login FTP Server
	int result = g_FtpClient.LogIn(ftpUrl);
	if(result != cftp::NOANYERROR)
		return result;

	string ip, userName, passwd, dir;
	string port = "21";
	string url = ftpUrl;
	result = g_FtpClient.ParseUrl(url, ip, userName, passwd, port, dir);
	if(result != cftp::NOANYERROR)
		return result;

	//string URL_PFX = url.substr(0,end);

	string  Dest = sFilmFilePath;	//���յ�DestĿ¼Ϊ"destĿ¼" + '/' + "pklUuid" + "/"

	ret = MakeDirectory2(Dest);		//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
	if(ret!=0)
	{
		error = "Error:Make Directory fail.\n";
		WELOG(error)
		ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
		return ret;
	}
	
	//dcp copy files
	if ( ret==0 )
	{
		int nAMInfoCount = dcpList.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AssetmapInfo aAMInfo = dcpList[i];
			vector< PklInfo > vDcp = aAMInfo.pklList;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				string pklName;

				PklInfo aPkl = vDcp[j];	
				if ( pklUuid == aPkl.pklUuid )
				{
					pklName = vDcp[j].pklName;
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif
					g_sCopyDcp_PklUuidName = pklUuid;
					g_Dcp_AllFile_Size = aPkl.allFileLength;
					g_hasDcpCopied_Size = 0ull;

					bIsFoundPKL = true;//pkl�ҵ�
					g_map_FileCopyInfo.clear();

					// 12/26/2012 zhangmiao begin 
					//��ʼ��g_map_FileCopyInfo ӳ��ṹ������pkl�������ļ�״̬��
					vector<CplInfo> vAsset_pkl = aPkl.cplList;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error)
						return errno;
					}
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						CplInfo aAsset_Pkl = vAsset_pkl[k];
						string fileName = aAsset_Pkl.cplName;
						if ( fileName.length()==0 )
						{
							error = "Error:Pkl AssetFile Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error)
							return errno;
						}

						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = Dest + fileName.substr(fileName.rfind('/')+1);
						aCopyInfo.fileSize = static_cast< int >(aAsset_Pkl.cplLength/ONEMB);
						aCopyInfo.bIsFinished = 0;
						g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind('/')+1)] = aCopyInfo;
					}
					
					vector<Mxf_FileInfo> vMxfInfo_pkl = aPkl.mxfList;
					nSize_Asset_Pkl = vMxfInfo_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error)
						return errno;
					}
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						Mxf_FileInfo aAsset_Pkl = vMxfInfo_pkl[k];
						string fileName = aAsset_Pkl.fileName;
						if ( fileName.length()==0 )
						{
							error = "Error:Pkl AssetFile mxf File Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error)
							return errno;
						}
						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = Dest + fileName.substr(fileName.rfind('/')+1);
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.length/ONEMB);
						aCopyInfo.bIsFinished = 0;
						g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind('/')+1)] = aCopyInfo;
					}

					//@author zhangmiao@oristartech.com
					//@date [2013-07-18]
					//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
					//@new
					vector<OtherFileInfo>& vFileInfoList_pkl = aPkl.fileInfoList;
					nSize_Asset_Pkl = vFileInfoList_pkl.size();
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						OtherFileInfo& aAsset_Pkl = vFileInfoList_pkl[k];
						string originalfileName = aAsset_Pkl.originalfileName;
						if ( originalfileName.length()==0 )
						{
							error = "Error:Pkl AssetFile OtherFile Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error);
							return errno;
						}
						
						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = Dest + originalfileName;
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.fileLength/ONEMB);
						aCopyInfo.bIsFinished = 0;
						g_map_FileCopyInfo[ Dest + originalfileName ] = aCopyInfo;
					}
					//@modify end;
					
#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					//------����copyInfo�е�ÿ���ļ�------
					ret = DownLoadDcp_BPTransSub1( Dest, aPkl, copyInfo , error );
					if ( ret != 0 )
					{

#ifdef _BP_DCPCOPYCANCEL_
						//����ϵ�����ʱ��DCP������;��ȡ��������ʱĿǰ��ɾ��δ�������DCPĿ¼�����Ժ����ӡ���ͣ���ܡ�ʱ�ٲ�ɾ��δ�������DCPĿ¼��
						//����ԭ�������Ŀ����жϣ���ɾ�������ļ��У�
						//if ( ret != ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL )
						{
							//ɾ�������ļ��У�Remove DCP Directory
							int nResult = RemoveDir( Dest );
							if( nResult != 0 )
							{
#ifdef _TEST_
								printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif //_TEST_
								error += "Warning:Remove DCP Directory fail.\n";
							}
						}
#endif //_BP_DCPCOPYCANCEL_

						return ret;
					}
					//----����copyInfo�е�ÿ���ļ�:end--------

					//Download PKL File
					ret = DownLoadDcp_BPTransSub2( pklName, Dest , error );
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return ret;
					}

					//Download AssetMap File
					string AssetmapName = aAMInfo.assetmapName;
					ret = DownLoadDcp_BPTransSub3( AssetmapName, pklUuid, Dest, aPkl , error );
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return ret;
					}

					if ( bIsFoundPKL && (errorOccurred==false) )
					{
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						errno = 0;
						g_sCopyDcp_PklUuidName="";
						g_sCopyDcp_Copying_FileName="";
						g_FtpClient.LogOut();

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						return 0;
					}
				}
			}
		}
		//Do not found pkl
		if (bIsFoundPKL==false)
		{
			//ɾ�������ļ��У�Remove DCP Directory
			ret = RemoveDir(Dest);
			if( ret != 0 )
			{
#ifdef _TEST_
				printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
				error += "Warning:Remove DCP Directory Fail.\n";
			}

#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif

			g_sCopyDcp_PklUuidName="";
			g_sCopyDcp_Copying_FileName="";
			g_hasDcpCopied_Size = g_Dcp_AllFile_Size = 0ull;
			g_FtpClient.LogOut();

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			error = "Do not found pkl\n";
			
			//return -1;
			WELOG(error)
			errno = ERROR_CONTENT_DCP_DONOT_FOUND_PKL;
			return errno;
		}
	}

	return 0;
}

//����copyInfo�е�ÿ���ļ�
int DownLoadDcp_BPTransSub1( const std::string &Dest, PklInfo& aPkl, std::vector<FileCopyInfo> &copyInfo, std::string& error )
{
	int ret(0);
	int nSize_Copy_Info = copyInfo.size();
	for( int s=0; s<nSize_Copy_Info; s++ )
	{
		FileCopyInfo& rFileCopyInfo = copyInfo[s];

		if (rFileCopyInfo.bIsFinished==1)		//���������ɣ��������һ���ļ���
		{
			//----zhangmiao:begin--[7/20/2012]----
			long long Length = 0;
			//@author zhangmiao@oristartech.com
			//@date [2013-07-18]
			//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
			//@new
			//ret = GetFileSize_PKLAssetList( rFileCopyInfo.filename , aPkl, Length );
			std::string originalfileName;
			ret = GetFileSize_PKLAssetList( rFileCopyInfo.filename , aPkl, Length , originalfileName );
			//@modify end;
			if (ret==0)
			{
				g_hasDcpCopied_Size += Length;
			}
			//----zhangmiao:end----[7/20/2012]----

			//���»�ȡ����ʱ��״̬
			//----zhangmiao:begin--20121226----
#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif
			//@author zhangmiao@oristartech.com
			//@date [2013-07-18]
			//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
			//@new
			//string destFileName = Dest + rFileCopyInfo.filename;
			string destFileName = Dest + originalfileName;
			//@modify end;
			map< string , FileCopyInfo >::size_type nCont = g_map_FileCopyInfo.count(destFileName);
			if ( nCont == 1 )
			{
				FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
				copyInfo.transferredSize = copyInfo.fileSize;
				copyInfo.bIsFinished = 1;
			}
#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			//----zhangmiao:end----20121226----

			continue;
		}

		string sFileCopyName = rFileCopyInfo.filename;
		if ( sFileCopyName.length()==0 )
		{
			error = "Error:FileCopyName is NULL!\n";
			//errno = -2;
			errno = ERROR_CONTENT_DCP_FILECOPYNAME_IS_NULL;
			WELOG(error)
			return errno;
		}

		//Download CPL File
		vector<CplInfo> vAsset_pkl = aPkl.cplList;
		int nSize_Asset_Pkl = vAsset_pkl.size();
		for ( int k=0; k< nSize_Asset_Pkl; k++ )
		{
			CplInfo aAsset_Pkl = vAsset_pkl[k];
			string fileName = aAsset_Pkl.cplName;
			if ( fileName.length()==0 )
			{
				error = "Error:CPL File Path Is NULL!\n";
				//errno = -2;
				errno = ERROR_CONTENT_DCP_CPL_FILE_PATH_IS_NULL;
				WELOG(error)
				return errno;
			}

			if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )    //�ж��ļ��Ƿ��ҵ�
			{
				//ֱ������CPL�ļ�
#ifdef WIN32
				WaitForSingleObject(hMutex,INFINITE);
#else
				pthread_mutex_lock(&work_mutex);
#endif

				g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif
				//check file exist
				vector<cftp::FileInformation> fileList;
				ret = listFilesOnFTP(g_FtpClient,fileName.substr(0,fileName.rfind('/')+1),fileList, fileName.substr(fileName.rfind('/')+1));
				if(ret != cftp::NOANYERROR)
					return ret;
				ret = ( fileList.size()!=0 && fileList[0].name==fileName );
				if( ret )
				{
#ifdef _TEST_
					printf( "File  exists :%s\n", fileName.c_str() );
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif	

					FileCopyInfo aCopyInfo;
					aCopyInfo.filename = Dest + fileName.substr(fileName.rfind('/')+1);
					aCopyInfo.fileSize = static_cast< int >(aAsset_Pkl.cplLength/ONEMB);
					aCopyInfo.transferredSize = 0;
					g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind('/')+1)]=aCopyInfo;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					//string srcFileName = URL_PFX  +  fileName;
					string destFileName = Dest + fileName.substr(fileName.rfind('/')+1);

					string dir = fileName.substr( 0, fileName.rfind( '/' ) );
					ret = g_FtpClient.TransportFile( dir  , Dest , fileName.substr( fileName.rfind( '/' )+1) );
					if (ret==0)
					{
						//----zhangmiao:begin--20121226----
						//���»�ȡ����ʱ��״̬��copy CPL file
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
						copyInfo.transferredSize = copyInfo.fileSize;
						copyInfo.bIsFinished = 1;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						//----zhangmiao:end----20121226----

						rFileCopyInfo.bIsFinished = 1;
						//----zhangmiao:begin--[7/20/2012]----
						g_hasDcpCopied_Size += aAsset_Pkl.cplLength;
						//----zhangmiao:end----[7/20/2012]----
#ifdef _TEST_
						printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
					}
					else
					{
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName="";
						g_Dcp_AllFile_Size = 0ull;

						if ( ret == CODE_DCPCOPYCANCEL )
						{
							g_flag_DcpCopyStop = 0;

							char buffer[BUF_SIZE]="";
							sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
							error = buffer;
#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

							errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
						}
						else
						{
#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

							errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
						}

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						//errno = 1;
						WELOG(error)
						return errno;
					}
				}
				else  //file NOT exist
				{
					char buffer[BUF_SIZE]="";
					sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
					error = buffer;
#ifdef _TEST_
					printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_PklUuidName="";
					g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					errno = ERROR_CONTENT_DCP_CPL_FILE_IS_NO_EXIST;
					WELOG(error)
					return errno;
				}


			}
			else
			{	//û���ҵ��������������һ���ļ�
				continue;
			}

		}

		//@author zhangmiao@oristartech.com
		//@date [2013-07-18]
		//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
		//@new
		//Download Other File
		vector<OtherFileInfo>& vFileInfoList_pkl = aPkl.fileInfoList;
		int nSize_OtherFileInfo_Asset_Pkl = vFileInfoList_pkl.size();
		for ( int k = 0; k < nSize_OtherFileInfo_Asset_Pkl; k++ )
		{
			OtherFileInfo& aAsset_Pkl = vFileInfoList_pkl[k];
			string fileName = aAsset_Pkl.fileName;
			string originalfileName = aAsset_Pkl.originalfileName;
			if ( fileName.length()==0 )
			{
				error = "Error:Pkl AssetFile Path Is NULL!\n";
				errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
				WELOG(error);
				return errno;
			}

			if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )
			{
				//ֱ��������������ļ���������Ļ�ļ�
#ifdef WIN32
				WaitForSingleObject(hMutex,INFINITE);
#else
				pthread_mutex_lock(&work_mutex);
#endif

				g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif

				//check file exist
				vector<cftp::FileInformation> fileList;
				ret = listFilesOnFTP(g_FtpClient,fileName.substr(0,fileName.rfind('/')+1),fileList, fileName.substr(fileName.rfind('/')+1));
				if(ret != cftp::NOANYERROR)
					return ret;
				ret = ( fileList.size()!=0 && fileList[0].name==fileName );
				if( ret )
				{
#ifdef _TEST_
					printf( "File  exists :%s\n", fileName.c_str() );
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif	

					FileCopyInfo aCopyInfo;
					aCopyInfo.filename = Dest + originalfileName;
					aCopyInfo.fileSize = static_cast< int >(aAsset_Pkl.fileLength/ONEMB);
					aCopyInfo.transferredSize = 0;
					g_map_FileCopyInfo[ Dest + originalfileName ] = aCopyInfo;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif

					//���Ŀ��Ŀ¼������Ŀ¼��������Ӧ��Ŀ¼��
					string SubPath;
					size_t pos = originalfileName.rfind('/');
					if ( pos != string::npos )
					{
						SubPath = originalfileName.substr( 0, pos ) + DIR_SEPCHAR;
					}
					string DestFile_InPath = Dest + SubPath;
					ret = MakeDirectory2(DestFile_InPath);		//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
					if(ret!=0)
					{
						error = "Error:DownLoadDcp_BPTransSub1--MakeDirectory2 fail.\n";
						WELOG(error);
						ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
						return ret;
					}

					
					string destFileName = Dest + originalfileName;

					string dir = fileName.substr( 0, fileName.rfind( '/' ) );
					ret = g_FtpClient.TransportFile( dir  , DestFile_InPath , fileName.substr( fileName.rfind( '/' )+1) );
					if (ret==0)
					{
						//���»�ȡ����ʱ��״̬��copy file �ɹ�
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
						copyInfo.transferredSize = copyInfo.fileSize;
						copyInfo.bIsFinished = 1;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif

						rFileCopyInfo.bIsFinished = 1;
						
						g_hasDcpCopied_Size += aAsset_Pkl.fileLength;
						
#ifdef _TEST_
						printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
					}
					else
					{
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName="";
						g_Dcp_AllFile_Size = 0ull;

						if ( ret == CODE_DCPCOPYCANCEL )
						{
							g_flag_DcpCopyStop = 0;

							char buffer[BUF_SIZE]="";
							sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
							error = buffer;
#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

							errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
						}
						else
						{
#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

							errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
						}

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						WELOG(error);
						return errno;
					}
				}
				else  //file NOT exist
				{
					char buffer[BUF_SIZE]="";
					sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
					error = buffer;
#ifdef _TEST_
					printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_PklUuidName="";
					g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					errno = ERROR_CONTENT_DCP_FILE_NOT_EXIST;
					WELOG(error);
					return errno;
				}
			}
			else
			{	//û���ҵ��������������һ���ļ�
				continue;
			}
		}
		//Download Other File end
		//@modify end;

		//Download mxf File
		vector<Mxf_FileInfo> vMxfInfo_pkl = aPkl.mxfList;
		int nSize_MxfInfo_Asset_Pkl = vMxfInfo_pkl.size();
		for ( int k=0; k< nSize_MxfInfo_Asset_Pkl; k++ )
		{
			Mxf_FileInfo aAsset_Pkl = vMxfInfo_pkl[k];
			string fileName = aAsset_Pkl.fileName;
			if ( fileName.length()==0 )
			{
				error = "Error:mxf File Path Is NULL!\n";
				//errno = -2;
				errno = ERROR_CONTENT_DCP_MXF_FILE_PATH_IS_NULL;
				WELOG(error)
				return errno;
			}

			if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )    //�ж��ļ��Ƿ��ҵ�
			{
				//����mxf�ļ�
#ifdef WIN32
				WaitForSingleObject(hMutex,INFINITE);
#else
				pthread_mutex_lock(&work_mutex);
#endif

				g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif
				//check file exist
				vector<cftp::FileInformation> fileList;
				ret = listFilesOnFTP(g_FtpClient,fileName.substr(0,fileName.rfind('/')+1),fileList, fileName.substr(fileName.rfind('/')+1));
				if(ret != cftp::NOANYERROR)
					return ret;
				ret = ( fileList.size()!=0 && fileList[0].name==fileName );
				if( ret  )
				{
#ifdef _TEST_
					printf( "File  exists :%s\n", fileName.c_str() );
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif
					FileCopyInfo aCopyInfo;
					aCopyInfo.filename = Dest + fileName.substr(fileName.rfind('/')+1);
					aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.length/ONEMB);
					aCopyInfo.transferredSize = 0;
					g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind('/')+1)]=aCopyInfo;

					//string srcFileName = URL_PFX  +  fileName;
					string destFileName = Dest + fileName.substr(fileName.rfind('/')+1);

					unsigned long long nlTransferredSize = 0;
					unsigned long long nfrom = 0;
					nlTransferredSize = rFileCopyInfo.transferredSize*ONEMB;
					nfrom = nlTransferredSize;
					//----zhangmiao:begin--[7/20/2012]----
					g_hasDcpCopied_Size += nlTransferredSize;
					//----zhangmiao:end----[7/20/2012]----
#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif

					string dir = fileName.substr( 0, fileName.rfind( '/' ) );

					ret = g_FtpClient.TransportFileOnBreakPoint( dir  , Dest , fileName.substr( fileName.rfind( '/' )+1) , nfrom );
					nlTransferredSize = g_FtpClient.FileTransportSize();
					rFileCopyInfo.transferredSize = static_cast<int>(nlTransferredSize/ONEMB);
					rFileCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.length/ONEMB);

					if (ret==0)
					{
						//----zhangmiao:begin--20121226----
						//���»�ȡ����ʱ��״̬��copy mxf file
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
						copyInfo.transferredSize = copyInfo.fileSize;
						copyInfo.bIsFinished = 1;

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						//----zhangmiao:end----20121226----

						rFileCopyInfo.bIsFinished = 1;
#ifdef _TEST_
						printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
					}
					else 
					{
#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName="";
						g_Dcp_AllFile_Size = 0ull;

						if ( ret == CODE_DCPCOPYCANCEL )
						{
							g_flag_DcpCopyStop = 0;

							char buffer[BUF_SIZE]="";
							sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
							error = buffer;

#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

							errno = ERROR_CONTENT_DCP_BP_DCPCOPYCANCEL;
						}
						else
						{
#ifdef _TEST_
							printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

							errno = ERROR_CONTENT_DCP_MXF_FILE_COPY_IS_UNFINISHED;
						}
							


#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						//errno = 1;
						WELOG(error)
						return errno;
					}
				}
				else  //file NOT exist
				{
					char buffer[BUF_SIZE]="";
					sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
					error = buffer;
#ifdef _TEST_
					printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif

					g_sCopyDcp_PklUuidName="";
					g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					errno = ERROR_CONTENT_DCP_MXF_FILE_IS_NO_EXIST;
					WELOG(error)
					return errno;
				}


			}
			else
			{	//û���ҵ��������������һ���ļ�
				continue;
			}

		}
	}
	return ret;
}

//Download PKL File
int DownLoadDcp_BPTransSub2( const std::string &pklName, const std::string &Dest, std::string& error )
{
	int ret(0);
	if (pklName.length())
	{
		vector<cftp::FileInformation> fileList;
		string fileName = pklName;
		ret = listFilesOnFTP(g_FtpClient,fileName.substr(0,fileName.rfind('/')+1), fileList, fileName.substr(fileName.rfind('/')+1));
		if(ret != cftp::NOANYERROR)
			return ret;
		ret = fileList.size()!=0 && ( fileList[0].name==fileName );    //check file exist,��ftp������������
		if( ret )
		{
			string destFileName = Dest + pklName.substr(pklName.rfind('/')+1);

			string dir = pklName.substr( 0, pklName.rfind( '/' ) );
			ret = g_FtpClient.TransportFile( dir  , Dest , pklName.substr( pklName.rfind( '/' )+1) );	//��������pkl�ļ�
			if (ret==0)
			{
#ifdef _TEST_
				printf( "PKL File copy finished :%s\n", destFileName.c_str() );
#endif
			}
			else if( ret )
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"PKL File copy Failed :%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "PKL File copy Failed :%s\n", destFileName.c_str() );
#endif
				//errno = 1;
				errno = ERROR_CONTENT_DCP_PKL_FILE_COPY_FAILED;
				WELOG(error)
				return errno;
			}
		}
		else
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Error:PKL File(%s) no exist!\n", pklName.c_str() );
			error = buffer;
#ifdef _TEST_
			printf( "Error:PKL File(%s) no exist!\n", pklName.c_str() );
#endif
			//errno = 10;
			errno = ERROR_CONTENT_DCP_PKL_FILE_NO_EXIST;
			WELOG(error)
			return errno;
		}
	}
	else
	{
		error = "Error:Param pklName is Null string!\n";
		//errno = -5;
		errno = ERROR_CONTENT_DCP_PARAM_PKLNAME_IS_NULL_STR;
		WELOG(error)
		return errno;
	}
	return ret;
}

//Download AssetMap File
int DownLoadDcp_BPTransSub3( const std::string &AssetmapName, const std::string &pklUuid, const std::string &Dest,
							  PklInfo& aPkl , std::string& error )
{
	int ret(0);
	if (AssetmapName.length())
	{
		vector<cftp::FileInformation> fileList;
		string fileName = AssetmapName;
		ret = listFilesOnFTP(g_FtpClient,fileName.substr(0,fileName.rfind('/')+1), fileList, fileName.substr(fileName.rfind('/')+1));
		if(ret != cftp::NOANYERROR)
			return ret;
		ret = fileList.size()!=0 && ( fileList[0].name==fileName );    //check file exist,��ftp������������
		if( ret )
		{
			string destFileName = Dest + AssetmapName.substr(AssetmapName.rfind('/')+1);

			string dir = AssetmapName.substr( 0, AssetmapName.rfind( '/' ) );
			ret = g_FtpClient.TransportFile( dir  , Dest , AssetmapName.substr( AssetmapName.rfind( '/' )+1) );		//��������AssetMap�ļ�
			if (ret==0)
			{
#ifdef _TEST_
				printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
			}
			else if( ret )
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"File copy Failed :%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "File copy Failed :%s\n", destFileName.c_str() );
#endif
				//errno = 1;
				errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
				WELOG(error)
				return errno;
			}

#if 1
			//----zhangmiao:begin---2012-12-10----
			//��Content::PklInfo& aPkl����Content::Dcp::pkl_Info_t�ṹ.
			Content::Dcp::pkl_Info_t aPkl_Info;
			aPkl_Info.uuid           = aPkl.pklUuid;
			aPkl_Info.annotationText = aPkl.AnnotationText;
			aPkl_Info.issueDate      = aPkl.IssueDate;
			aPkl_Info.issuer         = aPkl.Issuer;
			aPkl_Info.creator        = aPkl.Creator;
			aPkl_Info.allFileLength  = aPkl.allFileLength;

			int cplCount = aPkl.cplList.size();
			for ( int i = 0; i < cplCount; i++ )
			{
				asset_pkl_t aAsset_Pkl;
				CplInfo& aCpl = aPkl.cplList[i];
				aAsset_Pkl.uuid = aCpl.cplUuid;
				aPkl_Info.asset.push_back(aAsset_Pkl);
			}
			
			int mxfCount = aPkl.mxfList.size();
			for ( int j = 0; j < mxfCount; j++ )
			{
				asset_pkl_t aAsset_Pkl;
				Mxf_FileInfo aMxfInfo = aPkl.mxfList[j];
				aAsset_Pkl.uuid = aMxfInfo.mxfUuid;
				aPkl_Info.asset.push_back(aAsset_Pkl);
			}

			vector<OtherFileInfo>& fileInfoList = aPkl.fileInfoList;
			int otherCount = fileInfoList.size();
			for ( int k = 0; k < otherCount; k++ )
			{
				asset_pkl_t aAsset_Pkl;
				OtherFileInfo& aFileInfo = fileInfoList[k];
				aAsset_Pkl.uuid = aFileInfo.Uuid;
				aPkl_Info.asset.push_back(aAsset_Pkl);
			}
			
			//Rebuilt AssetMap File
			DcpInfo dcp;
			ret = dcp.ReBuild_Assetmap( destFileName ,pklUuid ,aPkl_Info , error );
			if (ret!=0)
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"Error ReBuild_Assetmap File FAILED:%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "Error ReBuild_Assetmap File FAILED:%s\n", destFileName.c_str() );
#endif
				WELOG(error);
				errno = ERROR_CONTENT_DCP_REBUILD_ASSETMAP_FILE_FAILED;
				ret = errno;
				return ret;
			}
			//----zhangmiao:end-----2012-12-10----
#endif

			//@author zhangmiao@oristartech.com
			//@date [2014-09-30]
			//@brief ͳһ���������Assetmap�ļ�����Ϊ�̶������֣�"ASSETMAP" 
			//@new
			int iRet = 0;
			string newDestFileName = Dest + "ASSETMAP";
			
			//@date [2015-04-17]
			//@brief �������SMPTE��ʽ��Dcp����ͳһ���������Assetmap�ļ�����Ϊ�̶������֣�"ASSETMAP.xml" 
			//@new
			if ( aPkl.nFilmPackageFormat == 1 )
			{
				newDestFileName = Dest + "ASSETMAP.xml";
			}
			//@modify [2015-04-17] end

			if ( newDestFileName != destFileName )
			{
				/* �����ļ��� */
				iRet = rename( destFileName.c_str() , newDestFileName.c_str() );
				if ( iRet == 0 )
				{
					printf("rename: %s to %s.\n", destFileName.c_str() , newDestFileName.c_str() );
				}
				else
				{
					perror("rename");
				}
			}
			//@modify end;
		}
		else
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Error:Assetmap(%s) is no exist!\n", AssetmapName.c_str() );
			error = buffer;
#ifdef _TEST_
			printf( "Error:Assetmap(%s) is no exist!\n", AssetmapName.c_str() );
#endif
			//errno = 3020;
			errno = ERROR_CONTENT_DCP_ASSETMAP_IS_NO_EXIST;
			WELOG(error)
			ret = errno;
		}
	}
	else
	{
		error = "Error:Param AssetmapName is Null string!\n";
#ifdef _TEST_
		printf( "Error:Param AssetmapName is Null string!\n" );
#endif
		//errno = 3010;
		errno = ERROR_CONTENT_DCP_PARAM_ASSETMAPNAME_IS_NULL_STR;
		WELOG(error)
		ret = errno;
		return ret;
	}

	return ret;
}

int CopyDcp( const string &source, const string &pklUuid, const string &dest, vector<FileCopyInfo> &copyInfo, std::string& error )
{
	//---���ڴ���ϵ����� zhangmiao [6/26/2012]
	bool bIsBPTrans = false;
	if( copyInfo.size()!=0 )
	{
		bIsBPTrans = true;
	}
	if (bIsBPTrans)
	{
		int ret = CopyDcp_BPTrans( source, pklUuid, dest, copyInfo, error );
		if( ret != 0 )
			ret = CMTT2(ret);
		return ret;
	}
	//----zhangmiao:end ---
	bool errorOccurred = false;
	bool bIsFoundPKL   = false;
	int ret(0);
	DcpInfo dcp;
	vector< AMInfo > aAMInfo_Vt;
	ret = dcp.CreateDCPInfo( source ,error );	//������ȵ�source��ָĿ¼��ȥ���������Ƿ�������ӰƬ��sourceĿ¼�����趨Ϊ����ƶ�Ӳ�̹���Ŀ¼��
	if(ret!=0)
	{
#ifdef _TEST_
		printf("error CreateDCPInfo !\n");
#endif
		error = "error:CreateDCPInfo()!\n" + error;
		WELOG(error)
		ret = CMTT(ERROR_CONTENT_DCP_CREATE_DCPINFO_ERROR);
		return ret;
	}
	//Parameters Check
	string sFilmFilePath = dest;
	if( sFilmFilePath.rfind(DIR_SEPCHAR) != sFilmFilePath.length()-1 )	//���destĿ¼��ĩβ�Ƿ���ָ��� "/" ��"\"�����������������ϡ�
		sFilmFilePath.append( 1, DIR_SEPCHAR );

	size_t pos;
	if ( ( pos = pklUuid.find("urn:uuid:") ) != pklUuid.npos && pos==0 )
	{
		sFilmFilePath = sFilmFilePath + pklUuid.substr(9);
	}
	else
	{
		sFilmFilePath = sFilmFilePath + pklUuid;
	}
	sFilmFilePath.append( 1, DIR_SEPCHAR );

	if ( sFilmFilePath.length()==0 )
	{
		error = "Error:FilmFilePath is NULL!\n";
		//errno = -2;
		WELOG(error)
		errno = ERROR_CONTENT_DCP_FILM_FILE_PATH_ISNULL;
		return CMTT2(errno);
	}

	string  Dest = sFilmFilePath;	//���յ�DestĿ¼Ϊ"destĿ¼" + '/' + "pklUuid" + "/"

	ret = MakeDirectory(Dest);
	if(ret!=0)
	{
		error = "Error:Make Directory fail.\n";
		WELOG(error)
		ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
		return CMTT2(ret);
	}
	else if ( ret==0 )   //dcp copy files
	{
		aAMInfo_Vt = dcp.vAMInfo_t;
		int nAMInfoCount = aAMInfo_Vt.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AMInfo_t aAMInfo = aAMInfo_Vt[i];
			vector<asset_assetmap_t> aAssetVt = aAMInfo.aAssetmap_Info.asset;
			vector< dcp_Info_t > vDcp = aAMInfo.vDcp;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				string pklName;
				//����pklName�ļ�·����
				int nAssetVtCount = aAssetVt.size();
				for ( int k=0; k< nAssetVtCount; k++ )
				{
					if ( pklUuid == aAssetVt[k].uuid )
					{
						pklName =  aAssetVt[k].path;
						if (pklName.length()==0)	//�������Ϊ�գ���������
						{
							char buffer[BUF_SIZE]="";
							sprintf( buffer,"pklName %s Error!\n", pklName.c_str() );
							error = buffer;
#ifdef _TEST_
							printf("pklName %s Error!\n", pklName.c_str());
#endif
							//return -1;
							//ɾ�������ļ��У�Remove DCP Directory
							int nResult = RemoveDir( Dest );
							if( nResult != 0 )
							{
#ifdef _TEST_
								printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
								error += "Warning:Remove DCP Directory fail.\n";
							}
							WELOG(error)
							errno = ERROR_CONTENT_DCP_PKL_PKLNAME_ERROR;
							return CMTT2(errno);
						}
#ifdef _TEST_
						printf( "pklName:%s\n", pklName.c_str() );
#endif
						break;
					}
				}

				dcp_Info_t dcpInf = vDcp[j];
				pkl_Info_t aPkl = dcpInf.aPkl;	
				if ( pklUuid == aPkl.uuid )
				{
#ifdef WIN32
					WaitForSingleObject(hMutex,INFINITE);
#else
					pthread_mutex_lock(&work_mutex);
#endif
					g_sCopyDcp_PklUuidName = pklUuid;
					g_Dcp_AllFile_Size = aPkl.allFileLength;
					g_hasDcpCopied_Size = 0ull;

					bIsFoundPKL = true;
					g_map_FileCopyInfo.clear();

					// [12/25/2012 zhangmiao begin ]
					//��ʼ�� g_map_FileCopyInfo ӳ��ṹ������pkl�������ļ�״̬��
					vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error)
						return errno;
					}
					for ( int m=0; m< nSize_Asset_Pkl; m++ )
					{
						asset_pkl_t aAsset_Pkl = vAsset_pkl[m];
						string fileName = aAsset_Pkl.originalFileName;
						if ( fileName.length() == 0 )
						{
							error = "Error:Pkl AssetFile Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error)
							return errno;
						}
					
						FileCopyInfo aCopyInfo;
						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
						//aCopyInfo.filename = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						aCopyInfo.filename =  fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
						//@modify end;
						aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);
						aCopyInfo.transferredSize = 0;
						//@author zhangmiao@oristartech.com
						//@date [2013-07-15]
						//��Ҫ�����������Ļ�ļ��Ĵ������
						//@new
						//g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1)] = aCopyInfo;
						g_map_FileCopyInfo[ fileName ] = aCopyInfo;
						//@modify end;
					}
					// [12/25/2012 zhangmiao end ]

#ifdef WIN32
					ReleaseMutex(hMutex);
#else
					pthread_mutex_unlock(&work_mutex);
#endif
					//Copy PKL AssetList File
					//@author zhangmiao@oristartech.com
					//@date [2013-07-15]
					//��Ҫ�����������Ļ�ļ��Ĵ������
					//@new
					//ret = CopyDcpSub1( Dest, aPkl ,error );
					ret = CopyDcpSub1( source , Dest , aPkl , error );
					//@modify end;
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return CMTT2(ret);
					}

					//Copy PKL File
					ret = CopyDcpSub2( pklName, Dest ,error );
					if ( ret != 0 )
					{
						//ɾ�������ļ��У�Remove DCP Directory
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return CMTT2(ret);
					}

					//Copy AssetMap File
					string AssetmapName = aAMInfo.assetmapName;
					ret = CopyDcpSub3( AssetmapName, pklUuid, Dest, dcp, aPkl ,error );
					if ( ret != 0 )
					{
						int nResult = RemoveDir( Dest );
						if( nResult != 0 )
						{
#ifdef _TEST_
							printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
							error += "Warning:Remove DCP Directory fail.\n";
						}

						return CMTT2(ret);
					}
					
					if ( bIsFoundPKL && (errorOccurred==false) )
					{
						errno = 0;

#ifdef WIN32
						WaitForSingleObject(hMutex,INFINITE);
#else
						pthread_mutex_lock(&work_mutex);
#endif

						g_sCopyDcp_PklUuidName="";
						g_sCopyDcp_Copying_FileName="";

#ifdef WIN32
						ReleaseMutex(hMutex);
#else
						pthread_mutex_unlock(&work_mutex);
#endif
						return 0;
					}
				}
			}
		}
		//Do not found pkl û���ҵ�PKLʱ������Ҫ�����⴦��
		if (bIsFoundPKL==false)
		{
			//ɾ�������ļ��У�Remove DCP Directory
			ret = RemoveDir(Dest);
			if( ret != 0 )
			{
#ifdef _TEST_
                printf( "Warning:Remove DCP Directory Fail.:%s\n", Dest.c_str() );
#endif
				error += "Warning:Remove DCP Directory Fail.\n";
			}

#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif

			g_sCopyDcp_PklUuidName="";
			g_sCopyDcp_Copying_FileName="";
			g_hasDcpCopied_Size = g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			error = "Do not found pkl\n";
			//return -1;
			WELOG(error)
			errno = ERROR_CONTENT_DCP_DONOT_FOUND_PKL;
			return CMTT2(errno);
		}
	}
	return 0;
}

//Copy PKL AssetList File
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int CopyDcpSub1( const std::string &Dest, pkl_Info_t& aPkl, std::string& error )
int CopyDcpSub1( const std::string &source, const std::string &Dest, pkl_Info_t& aPkl, std::string& error )
//@modify end;
{
	int ret(0);
	vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
	int nSize_Asset_Pkl = vAsset_pkl.size();
	if (nSize_Asset_Pkl==0)
	{
		error = "Error:PKL AssetList is null!\n";
		//errno = 1700;
		errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
		WELOG(error)
		ret = errno;
	}
	
	for ( int k=0; k< nSize_Asset_Pkl; k++ )
	{
		asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
		//string hashValue = aAsset_Pkl.hash;
		string fileName = aAsset_Pkl.originalFileName;
		if ( fileName.length()==0 )
		{
			error = "Error:Pkl AssetFile Path Is NULL!\n";
			//errno = -2;
			errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
			WELOG(error)
			return errno;
		}
#ifdef WIN32
		WaitForSingleObject(hMutex,INFINITE);
#else
		pthread_mutex_lock(&work_mutex);
#endif
		g_sCopyDcp_Copying_FileName = fileName;

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

		if( ( ret = access( fileName.c_str() , F_OK ) ) == 0 )     //check file exist
		{
#ifdef _TEST_
			printf( "File  exists :%s\n", fileName.c_str() );
#endif
			//printf( "Delete file :%s\n", fileName.c_str() );
			//ret = remove( fileName.c_str() );

			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//@new
/*
#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif
			FileCopyInfo aCopyInfo;
			aCopyInfo.filename = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
			aCopyInfo.fileSize = static_cast<int>(aAsset_Pkl.size/ONEMB);
			aCopyInfo.transferredSize = 0;
			g_map_FileCopyInfo[Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1)] = aCopyInfo;
#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif			
*/
			//@modify end;

			string srcFileName = fileName;
			
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//@new
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//string destFileName = Dest + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);

			//���Ŀ��Ŀ¼������Ŀ¼��������Ӧ��Ŀ¼��
			string tmpPath,SubPath;
			size_t pos1 = fileName.rfind( source );
			if ( pos1 != string::npos )
			{
				tmpPath = fileName.substr( pos1 + source.length() );
			}
			size_t pos2 = tmpPath.rfind(DIR_SEPCHAR);
			if ( pos2 != string::npos )
			{
				SubPath = tmpPath.substr( 0, pos2+1 );
			}
			
			string DestFile_InPath = Dest + SubPath;
			ret = MakeDirectory2(DestFile_InPath);		//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
			if(ret!=0)
			{
				error = "Error:CopyDcpSub1--Make Director2 fail.\n";
				WELOG(error)
				ret = ERROR_CONTENT_DCP_MAKE_DIR_FAIL;
				return ret;
			}

			string destFileName = DestFile_InPath + fileName.substr(fileName.rfind(DIR_SEPCHAR)+1);
			//@modify end;

			ret = cp_file_Mode2( srcFileName.c_str(), destFileName.c_str() );
			if (ret==0)
			{
				//----zhangmiao:begin--20121226----
				//���»�ȡ����ʱ��״̬��Ϣ��copy file �ɹ�ʱ.
#ifdef WIN32
				WaitForSingleObject(hMutex,INFINITE);
#else
				pthread_mutex_lock(&work_mutex);
#endif

				//@author zhangmiao@oristartech.com
				//@date [2013-07-15]
				//��Ҫ�����������Ļ�ļ��Ĵ������
				//@new
				//FileCopyInfo& copyInfo   = g_map_FileCopyInfo[destFileName]; 
				FileCopyInfo& copyInfo   = g_map_FileCopyInfo[srcFileName]; 
				//@modify end;
				copyInfo.bIsFinished = 1;

#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif
				//----zhangmiao:end----20121226----

#ifdef _TEST_
				printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
			}
			else 
			{
#ifdef WIN32
				WaitForSingleObject(hMutex,INFINITE);
#else
				pthread_mutex_lock(&work_mutex);
#endif

				g_sCopyDcp_PklUuidName="";
				g_Dcp_AllFile_Size = 0ull;

				if ( ret == CODE_DCPCOPYCANCEL )
				{
					g_flag_DcpCopyStop = 0;

					char buffer[BUF_SIZE]="";
					sprintf( buffer,"%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!" );
					error = buffer;
#ifdef _TEST_
					printf("%s:%s\n",fileName.c_str(),"DCP Copy CANCEL!");
#endif

					errno = ERROR_CONTENT_DCP_DCPCOPYCANCEL;
				}
				else
				{
#ifdef _TEST_
					printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

					errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
				}

#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif
				
				//errno = 1;
				WELOG(error)
				return errno;
			}
		}
		else//file NOT exist
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"%s:%s\n",fileName.c_str(),strerror(errno) );
			error = buffer;
#ifdef _TEST_
			printf("%s:%s\n",fileName.c_str(),strerror(errno));
#endif

#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif

			g_sCopyDcp_PklUuidName="";
			g_Dcp_AllFile_Size = 0ull;

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
			errno = ERROR_CONTENT_DCP_FILE_NOT_EXIST;
			WELOG(error)
			return errno;
		}
	}

	return ret;
}

//Copy PKL File
int CopyDcpSub2( const std::string &pklName, const std::string &Dest, std::string& error )
{
	int ret(0);
	if (pklName.length())
	{
		if( ( ret = access( pklName.c_str() , F_OK ) ) == 0 )     //check file exist
		{
			string destFileName = Dest + pklName.substr(pklName.rfind(DIR_SEPCHAR)+1);
			ret = cp_file( pklName.c_str(), destFileName.c_str() );
			if (ret==0)
			{
#ifdef _TEST_
				printf( "PKL File copy finished :%s\n", destFileName.c_str() );
#endif
			}
			else if( ret )
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"PKL File copy Failed :%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "PKL File copy Failed :%s\n", destFileName.c_str() );
#endif
				//errno = 1;
				errno = ERROR_CONTENT_DCP_PKL_FILE_COPY_FAILED;
				WELOG(error)
				return errno;
			}
		}
		else
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Error:PKL File(%s) no exist!\n", pklName.c_str() );
			error = buffer;
#ifdef _TEST_
			printf( "Error:PKL File(%s) no exist!\n", pklName.c_str() );
#endif
			//errno = 10;
			errno = ERROR_CONTENT_DCP_PKL_FILE_NO_EXIST;
			WELOG(error)
			return errno;
		}
	}
	else
	{
		error = "Error:Param pklName is Null string!\n";
#ifdef _TEST_
		printf( "Error:Param pklName is Null string!\n" );
#endif
		//errno = -5;
		errno = ERROR_CONTENT_DCP_PARAM_PKLNAME_IS_NULL_STR;
		WELOG(error)
		return errno;
	}
	return ret;
}

//Copy AssetMap File ���������Ҫ�ؽ�AssetMap�ļ�
int CopyDcpSub3( const std::string &AssetmapName, const std::string &pklUuid, const std::string &Dest,
				DcpInfo& dcp, pkl_Info_t& aPkl , std::string& error )
{
	int ret(0);
	if (AssetmapName.length())
	{
		if( ( ret = access( AssetmapName.c_str() , F_OK ) ) == 0 )
		{
			//@author zhangmiao@oristartech.com
			//@date [2014-09-30]
			//@brief ͳһ���������Assetmap�ļ�����Ϊ�̶������֣�"ASSETMAP" 
			//@new
			//string destFileName = Dest + AssetmapName.substr(AssetmapName.rfind(DIR_SEPCHAR)+1);
			string destFileName = Dest + "ASSETMAP";
			//@modify [2014-09-30] end;

			//@author zhangmiao@oristartech.com
			//@date [2015-04-17]
			//@brief �������SMPTE��ʽ��Dcp����ͳһ���������Assetmap�ļ�����Ϊ�̶������֣�"ASSETMAP.xml" 
			//@new
			if ( aPkl.nFilmPackageFormat == 1 )
			{
				destFileName = Dest + "ASSETMAP.xml";
			}
			//@modify [2015-04-17] end;
			
			ret = cp_file( AssetmapName.c_str(), destFileName.c_str() );
			if (ret==0)
			{
#ifdef _TEST_
				printf( "File copy finished :%s\n", destFileName.c_str() );
#endif
			}
			else if( ret )
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"File copy Failed :%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "File copy Failed :%s\n", destFileName.c_str() );
#endif
				//errno = 1;
				errno = ERROR_CONTENT_DCP_FILE_COPY_FAILED;
				WELOG(error)
				return errno;
			}
			//Rebuilt AssetMap File
			ret = dcp.ReBuild_Assetmap( destFileName ,pklUuid ,aPkl , error );
			if (ret!=0)
			{
				char buffer[BUF_SIZE]="";
				sprintf( buffer,"Error ReBuild_Assetmap File FAILED:%s\n", destFileName.c_str() );
				error = buffer;
#ifdef _TEST_
				printf( "Error ReBuild_Assetmap File FAILED:%s\n", destFileName.c_str() );
#endif
				WELOG(error)
				errno = ERROR_CONTENT_DCP_REBUILD_ASSETMAP_FILE_FAILED;
				ret = errno;
				return ret;
			}
		}
		else
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Error:Assetmap(%s) is no exist!\n", AssetmapName.c_str() );
			error = buffer;
#ifdef _TEST_
			printf( "Error:Assetmap(%s) is no exist!\n", AssetmapName.c_str() );
#endif
			//errno = 2020;
			errno = ERROR_CONTENT_DCP_ASSETMAP_IS_NO_EXIST;
			WELOG(error)
			ret = errno;
		}
	}
	else
	{
		error = "Error:Param AssetmapName is Null string!\n";
#ifdef _TEST_
		printf( "Error:Param AssetmapName is Null string!\n" );
#endif
		//errno = 2022;
		errno = ERROR_CONTENT_DCP_PARAM_ASSETMAPNAME_IS_NULL_STR;
		WELOG(error)
		ret = errno;
		return ret;
	}
	return ret;
}

//FTP����DCP
int DownLoadDcp( const string &ftpUrl, const string &pklUuid, const string &dest, vector<FileCopyInfo> &copyInfo, string& error )
{
	int ret(0);
	bool bIsBPTrans = false;
	if( copyInfo.size()!=0 )
	{
		bIsBPTrans = true;
	}
	else
	{
		//@author zhangmiao@oristartech.com
		//@date 2013-02-27
		//@brief �������copyInfoΪ��������Զ����cpl��mxf�ļ�����:begin
		//@new
		//������Search Ftp DcpList��
		vector<AssetmapInfo> dcpList;
		ret = SearchFtpDcpList( ftpUrl, dcpList ,error );
		if(ret!=0)
		{
#ifdef _TEST_
			printf("error SearchFtpDcpList !\n");
#endif
			WELOG("error SearchFtpDcpList !\n");
			return ret;
		}

		int nAMInfoCount = dcpList.size();
		for ( int i=0; i< nAMInfoCount; i++ )
		{
			AssetmapInfo aAMInfo = dcpList[i];
			vector< PklInfo > vDcp = aAMInfo.pklList;
			int pklN = vDcp.size();
			for( int j=0; j<pklN; j++ )
			{
				//string pklName;

				PklInfo aPkl = vDcp[j];	
				if ( pklUuid == aPkl.pklUuid )
				{
					//pklName = vDcp[j].pklName;
					//���cpl�ļ���Ϣ����
					vector<CplInfo> vAsset_pkl = aPkl.cplList;
					int nSize_Asset_Pkl = vAsset_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error);
						return errno;
					}
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						CplInfo aAsset_Pkl = vAsset_pkl[k];
						string fileName = aAsset_Pkl.cplName;
						if ( fileName.length() == 0 )
						{
							error = "Error:Pkl AssetFile Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error);
							return errno;
						}

						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = fileName.substr(fileName.rfind('/')+1);
						aCopyInfo.bIsFinished = 0;
						copyInfo.push_back(aCopyInfo);
					}

					//���mxf�ļ���Ϣ����
					vector<Mxf_FileInfo> vMxfInfo_pkl = aPkl.mxfList;
					nSize_Asset_Pkl = vMxfInfo_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Error:PKL AssetList is null!\n";
						errno = ERROR_CONTENT_DCP_PKL_ASSELIST_IS_NULL;
						WELOG(error);
						return errno;
					}
					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						Mxf_FileInfo aAsset_Pkl = vMxfInfo_pkl[k];
						string fileName = aAsset_Pkl.fileName;
						if ( fileName.length() == 0 )
						{
							error = "Error:Pkl AssetFile mxf File Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error);
							return errno;
						}
						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = fileName.substr(fileName.rfind('/')+1);
						aCopyInfo.bIsFinished = 0;
						copyInfo.push_back(aCopyInfo);
					}

					//@author zhangmiao@oristartech.com
					//@date [2013-07-18]
					//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
					//@new
					//��������ļ���Ϣ����
					//��Ҫ����Ļ��ص��ļ�������Ļ�ļ�����û��
					vector<OtherFileInfo>& vFileInfoList_pkl = aPkl.fileInfoList;
					nSize_Asset_Pkl = vFileInfoList_pkl.size();
					if (nSize_Asset_Pkl==0)
					{
						error = "Subtitle files list is null!\n";
						printf( "Warning:%s\n" , error.c_str() );
					}

					vector<string> vTmp;

					for ( int k=0; k< nSize_Asset_Pkl; k++ )
					{
						OtherFileInfo aAsset_Pkl = vFileInfoList_pkl[k];
						string fileName = aAsset_Pkl.fileName;
						if ( fileName.length() == 0 )
						{
							error = "Error:Pkl AssetFile File Path Is NULL!\n";
							errno = ERROR_CONTENT_DCP_PKL_ASSEFILE_PATH_IS_NULL;
							WELOG(error);
							return errno;
						}

						FileCopyInfo aCopyInfo;
						aCopyInfo.filename = fileName.substr(fileName.rfind('/')+1);
						aCopyInfo.bIsFinished = 0;

						int result = count(vTmp.begin(),vTmp.end(), aCopyInfo.filename );
						if( result != 0 ) 
							continue;
						else   
							vTmp.push_back(aCopyInfo.filename);

						copyInfo.push_back(aCopyInfo);
					}
					//@modify end;

				}
			}
		}
		//�������copyInfoΪ��������Զ����cpl��mxf�ļ�����
		//@modify end;

		if( copyInfo.size() != 0 )
		{
			bIsBPTrans = true;
		}
		else
		{
			error = "Param copyInfo is NULL!\n";	
			WELOG(error);	//ret = -1;
			ret = CMTT2(ERROR_CONTENT_DCP_PARAM_COPYINFO_ISNULL);
		}
	}
	if (bIsBPTrans)
	{
		ret = DownLoadDcp_BPTrans( ftpUrl, pklUuid, dest, copyInfo, error );
		if( ret != 0 )
			ret = CMTT2(ret);
		return ret;
	}
	return ret;
}
//ȡ��FTP����DCP
int CancelDownLoad( const string &ftpUrl, const string &pklUuid, string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	if (g_sCopyDcp_PklUuidName==pklUuid)
	{
		if ( g_flag_DcpCopyStop == 0 )
		{
			g_flag_DcpCopyStop = 1;

			int result = g_FtpClient.CancelTransFile();
			g_FtpClient.LogOut();
			if(result != cftp::NOANYERROR)
			{
#ifdef WIN32
				ReleaseMutex(hMutex);
#else
				pthread_mutex_unlock(&work_mutex);
#endif
				error = "FtpClient::CancelTransFile() Error!\n";
				WELOG(error)
				result = ERROR_CONTENT_FTP_CANCEL_TRANSFILE_ERROR;
				return CMTT2(result);
			}
				
		}
		else if ( g_flag_DcpCopyStop == 1 && errno==CODE_DCPCOPYCANCEL )
		{
			g_flag_DcpCopyStop = 0;
			g_sCopyDcp_PklUuidName="";
		}
	}
	else
	{
		g_sCopyDcp_PklUuidName="";

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif
		error = "PklUuid is error.";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_DCP_PKL_UUID_ERROR);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}
//ȡ��FTP����DCP����
int GetDownLoadPercent(const string &ftpUrl, const string &pklUuid, CopyInfo &info , std::string& error )
{
#ifdef WIN32
	WaitForSingleObject(hMutex,INFINITE);
#else
	pthread_mutex_lock(&work_mutex);
#endif

	int nPercent = 0;
	if ( g_sCopyDcp_PklUuidName == pklUuid )
	{
		long double  fRate = ((long double)(g_hasDcpCopied_Size)) / ((long double)(g_Dcp_AllFile_Size));
		nPercent = int(fRate*100.0);
		if (nPercent==100)
		{
			info.status = DCP_STATUS_FINISHED;
		}
		else
		{
			info.status = DCP_STATUS_COPING;
		}

		if ( errno != 0 )
		{
			error = "error:DCP DownLoad Percent is DCP_STATUS_ERROR.";
			info.status = DCP_STATUS_ERROR;
			WELOG(error)
		}

		info.percent = nPercent;

		info.dest = "";
		info.source = ftpUrl;

		if (g_map_FileCopyInfo.size())
		{
			info.copyInfo.clear();
			map< std::string , FileCopyInfo >::iterator iFirst = g_map_FileCopyInfo.begin();
			map< std::string , FileCopyInfo >::iterator iLast = g_map_FileCopyInfo.end();
			for (; iFirst != iLast; ++iFirst)
				info.copyInfo.push_back((*iFirst).second);
		}
	}
	else
	{
		info.status = DCP_STATUS_NOSTART;
		info.percent = 0;
		info.source = ftpUrl;
		info.copyInfo.clear();

#ifdef WIN32
		ReleaseMutex(hMutex);
#else
		pthread_mutex_unlock(&work_mutex);
#endif

		error = "error:DCP DownLoad is NO START.";
		//return -1;
		WELOG(error)
		return CMTT2(ERROR_CONTENT_DCP_COPYDOWNLOAD_NO_START);
	}

#ifdef WIN32
	ReleaseMutex(hMutex);
#else
	pthread_mutex_unlock(&work_mutex);
#endif

	return 0;
}

//��ȡPKL���ʲ��б�AssetList�У�ָ���ļ�FileName�Ĵ�С��FileName���ļ���������·����
//����
//@author zhangmiao@oristartech.com
//@date [2013-07-15]
//��Ҫ�����������Ļ�ļ��Ĵ������
//@new
//int GetFileSize_PKLAssetList( const std::string FileName ,const Content::Dcp::pkl_Info_t& aPkl, long long& Length )
int GetFileSize_PKLAssetList( const string FileName ,const pkl_Info_t& aPkl, long long& Length , string& FullPathFileName )
//@modify end;
{
	int ret(0);
	bool IsFound = false;

	string sFileCopyName = FileName;
	if ( sFileCopyName.length()==0 )
	{
		errno = -2;
		return errno;
	}

	vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
	int nSize_Asset_Pkl = vAsset_pkl.size();
	for ( int k=0; k< nSize_Asset_Pkl; k++ )
	{
		asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
		
		string fileName = aAsset_Pkl.originalFileName;
		if ( fileName.length()==0 )
		{
			errno = -2;
			return errno;
		}

		if ( FileName == fileName.substr(fileName.rfind(DIR_SEPCHAR)+1) )    //�ж��ļ��Ƿ��ҵ�
		{
			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//��Ҫ�����������Ļ�ļ��Ĵ������
			//@new
			FullPathFileName = fileName;
			//@modify end;
			Length = aAsset_Pkl.size;
			if ( Length != 0 )
			{
				IsFound = true;
				break;
			}
		}
	}

	if ( IsFound == false )
	{
		return -1;
	}

	return ret;
}
//��ȡPKL���ʲ��б�AssetList�У�ָ���ļ�FileName�Ĵ�С��FileName���ļ���������·����
//ftp DownLoad ʱ��ʹ��
//@author zhangmiao@oristartech.com
//@date [2013-07-18]
//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
//@new
//int GetFileSize_PKLAssetList( const std::string FileName ,const PklInfo& aPkl, long long& Length  )
int GetFileSize_PKLAssetList( const std::string FileName ,const PklInfo& aPkl, long long& Length ,std::string& originalfileName )
//@modify end;
{
	int ret(0);
	bool IsFound = false;

	string sFileCopyName = FileName;
	if ( sFileCopyName.length()==0 )
	{
		errno = -2;
		return errno;
	}

	//�ȴ�CPL File����
	vector<CplInfo> vAsset_pkl = aPkl.cplList;
	int nSize_Asset_Pkl = vAsset_pkl.size();
	for ( int k=0; k< nSize_Asset_Pkl; k++ )
	{
		CplInfo aAsset_Pkl = vAsset_pkl[k];
		string fileName = aAsset_Pkl.cplName;
		if ( fileName.length()==0 )
		{
			errno = -2;
			return errno;
		}

		if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )    //�ж��ļ��Ƿ��ҵ�
		{
			Length = aAsset_Pkl.cplLength;
			if ( Length != 0 )
			{
				IsFound = true;
				return 0;
			}
		}
	}

	//�ٴ� MXF File����
	vector<Mxf_FileInfo> vMxfInfo_pkl = aPkl.mxfList;
	int nSize_MxfInfo_Asset_Pkl = vMxfInfo_pkl.size();
	for ( int k=0; k< nSize_MxfInfo_Asset_Pkl; k++ )
	{
		Mxf_FileInfo aAsset_Pkl = vMxfInfo_pkl[k];
		string fileName = aAsset_Pkl.fileName;
		if ( fileName.length()==0 )
		{
			errno = -2;
			return errno;
		}

		if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )    //�ж��ļ��Ƿ��ҵ�
		{
			Length = aAsset_Pkl.length;
			if ( Length != 0 )
			{
				IsFound = true;
				return 0;
			}
		}
	}

	//@author zhangmiao@oristartech.com
	//@date [2013-07-18]
	//@brief ��Ҫ������FTP���������Ļ�ļ��Ĵ������
	//@new
	//����Ļ����ļ���Ѱ��
	vector<OtherFileInfo> vFileInfoList_pkl = aPkl.fileInfoList;
	int nSize_OtherFileInfo_Asset_Pkl = vFileInfoList_pkl.size();
	for ( int k = 0; k < nSize_OtherFileInfo_Asset_Pkl; k++ )
	{
		OtherFileInfo aAsset_Pkl = vFileInfoList_pkl[k];
		string fileName = aAsset_Pkl.fileName;
		if ( fileName.length()==0 )
		{
			errno = -2;
			return errno;
		}

		if ( sFileCopyName == fileName.substr(fileName.rfind('/')+1) )    //�ж��ļ��Ƿ��ҵ�
		{
			originalfileName = aAsset_Pkl.originalfileName;
			Length = aAsset_Pkl.fileLength;
			if ( Length != 0 )
			{
				IsFound = true;
				return 0;
			}
		}
	}
	//@modify end;

	//�����û�ҵ����򷵻�-1
	if ( IsFound == false )
	{
		return -1;
	}

	return ret;
}

//@author zhangmiao@oristartech.com
//@date 2015-05-05
//@brief ���Ӵ��룬�ж���Ƶ���ͣ�nVideoType��ֵ:1����Mpeg2��0����2D��ʽ��Jpeg2000
//#include "CMXFParserModule.h"

// The length of UUID.
const unsigned int UUID_LENGTH = 16;

// This struct include MXF file's informations.
struct MxfInfo
{
	const char *m_cFileName;           // Mxf file name.

	/// XLQ:����1����Mpeg2��2����2D��ʽ��Jpeg2000
	int m_iVideoType;

	unsigned long m_uFrameSum;         // The sum frame number of mxf file.
	unsigned long m_uFrameRate;        // The frame rate of mxf file.
	bool m_bCryptoFlag;                // The CryptoFlag of mxf file.
	bool m_bHmacFlag;				   // The HMACFlag of mxf file.
	char m_cKeyID[UUID_LENGTH];        // The Key ID of mxf file.
	char m_cAssetUuid[UUID_LENGTH];    // The Asset UUID of mxf file.

	double m_dAspectRatio;             // The aspect rate of mxf file.
	unsigned long m_uWidthSize;        // The width size of video.
	unsigned long m_uHeightSize;       // The height size of video.

	unsigned long m_uSamplingRate;     // The sampling rate of mxf file.
	unsigned long m_uChannelCount;     // The channel count of mxf file.
	unsigned long m_uBitsPerSample;    // The BitsPerSample of mxf file.

};

int CheckVideoType( VIDEO_TYPE &rVideoType , const std::string &sPicUuid , const std::vector< Content::Dcp::asset_pkl_t >& vAsset_pkl )
{
	bool bIsInvalid  = false;
	string fileName ;
	string asset_cpl_Uuid = sPicUuid;

	std::vector< Content::Dcp::asset_pkl_t >::size_type  nSize_Asset_Pkl = vAsset_pkl.size();

	std::vector< Content::Dcp::asset_pkl_t >::size_type k = 0;
	for ( k = 0; k < nSize_Asset_Pkl; ++k )
	{
		asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
		string &Uuid_Asset_Pkl = aAsset_Pkl.uuid;

		if ( Uuid_Asset_Pkl == asset_cpl_Uuid )
		{
			fileName = aAsset_Pkl.originalFileName;
			//����ļ���С,assetLength
			unsigned long long assetLength = 0;
			struct stat stsrc;
			if( lstat( fileName.c_str(), &stsrc ) == 0 )
				assetLength = stsrc.st_size;
			else{

				bIsInvalid = true;

#ifdef _TEST_
				printf("lstat %s is error:%s!\n",fileName.c_str(),strerror(errno));
#endif
				
				break;
			}
			//�Ƚ��ļ���С
			unsigned long long filesize = aAsset_Pkl.size;
			if ( filesize != assetLength )
			{
				bIsInvalid = true;
				break;
			}
			break;
		}
	}

	//���ֳ������⣬������ֹ��顣
	if ( bIsInvalid == true )
	{
		return bIsInvalid;
	}

	//int iRet = -1;
	//unsigned long videoFrameSum = 0;
	//video info
	int m_iVideoType = -1;
	/*
	double m_dAspectRatio = 0;
	unsigned long m_uWidthSize = 0;
	unsigned long m_uHeightSize = 0;
	unsigned long m_uFrameRate = 0;
	bool m_bHmacFlag1 = false;
	bool m_bCryptoFlag1 = false; 
	char m_cKeyID1[16] = "";
	*/
#if 0

	CMXFParserModule cmxfParserModule;
	iRet = cmxfParserModule.InitVideoParser( fileName.c_str() , videoFrameSum );
	if ( iRet != 0 )
	{
		return iRet;
	}
	
	printf("video sum frame:%lu\n", videoFrameSum);

	iRet = cmxfParserModule.GetVideoInfo(m_iVideoType, m_dAspectRatio, m_uWidthSize, m_uHeightSize, m_uFrameRate, m_bHmacFlag1, m_bCryptoFlag1, m_cKeyID1);
	if ( iRet != 0 )
	{
		return iRet;
	}
#endif	
	printf("Video info :\n");
	printf("VideoType(1=Mpeg2,2=2D Jpeg2k):[%d]\n",m_iVideoType); //video����

	if ( m_iVideoType == 1 )
	{
		rVideoType = VIDEO_TYPE_MPEG2;
	}
	else if ( m_iVideoType == 2 )
	{
		rVideoType = VIDEO_TYPE_JPEG2000;
	}

	return 0;
}
//@modify [2015-05-05] end;

#if defined(TMS2_CM_ERRCODE) || defined(WRITE_ELOG) 
#ifndef TMS20_LOG
#include "log/C_LogManage.h"
#endif
#endif


#ifdef TMS2_CM_ERRCODE
//���������֣����ݹ���ʹ��1001���ϵ�ֵ��
int ContentErrorCodeToTmsErrorCode(int error)
{
	const int nCONTENT_ErrorLevel = 3;
	const int nCONTENT_Module = 19;
	const int nCONTENT_SubModule = 0;
	const int nCONTENT_ERROR_Code_Base_Value = 1000;

	if(error == 0)
		return 0;
	else if(error < 0xFFFF)
		return C_LogManage::GetInstance()->CreateLogNumber(nCONTENT_ErrorLevel, nCONTENT_Module, nCONTENT_SubModule, nCONTENT_ERROR_Code_Base_Value + abs(error));
	else
		return error;
}
#endif // TMS2_CM_ERRCODE

#ifdef WRITE_ELOG
int WriteErrorLog( const std::string& strError )
{
	int ret = 0;
	int iLevel=1, iModule=19, iSubModule=0;
	int errorCode = 0;
	ret = C_LogManage::GetInstance()->WriteLog( iLevel,  iModule, iSubModule, errorCode, strError);
	return ret;
}
#endif // WRITE_ELOG
