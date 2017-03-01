#include <iostream>
#include <fstream>

#include "soapH.h"
#include "ns1.nsmap"


#ifndef __CONTENT_H__
#include "content.h"
#endif

#include <strings.h>
#include <sys/statfs.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sstream>
#include <queue>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "Popen.h"
#include "soapclientprocess.h"
#include "config.h"
#include "LogManage.h"
#include "rmDir.h"

extern "C"
{
	#include "sqlite3.h"
}

extern std::string store_g;
extern std::string temp_g;
extern std::string torrent_g;
extern std::string zip_g;
extern std::string tracker_g;
extern std::string dom_g;
extern LogManage Log;
extern sqlite3 *db_g;
const int bufferSize = 128;


std::queue<struct ns1__InterfaceParameter> Que;
pthread_mutex_t  mutex_ =  PTHREAD_MUTEX_INITIALIZER;

using namespace ::Content;
using namespace ::Content::Dcp;
using namespace std;

/*******************************************************************/
bool ContainAlpha(const std::string& str)
{
	size_t i = 0;
	bool result = false;
	
	for(; i < str.size(); ++i)
	{
		if( (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )	
			result = true;
	}	
	
	return result;
}

/*If Folder exit then return true, else retrun false*/
bool IsFolderExist( const string& path )
{
    DIR *dp;
    if ((dp = opendir( path.c_str() )) == NULL)
    {
        return false;
    }

    closedir(dp);
    return true;
}

char *Letter_strlwr(char *s)
{
	char *p=s;
	while(*s!='\0')
	{
		if(*s>='A'&&*s<='Z')
			*s+=32;
		s++;
	}
	return p;
}

string ltos(long long l)
{
	ostringstream os;
	os << l;
	
	string result;
	istringstream is(os.str());
	is>>result;
	
	return result;
}


int makeToAndIssueFunc( const string& packUuid )
{
	int iRet(0);
	
	string error;
	string argo = packUuid;
	
	/*1.制作种子*/
	string pythonCommd = "python " + string("./../BitTorrent-4.0.3/") + "btmaketorrent.py" + 
	                     string(" http://"+tracker_g+":6969/announce ") + store_g + packUuid;
	
	Log.WriteLog(LOG_INFO, pythonCommd.c_str() );	
	if( Popen( pythonCommd.c_str() ) < 0)
	{
		Log.WriteLog(LOG_INFO, "make torrent failed!\n" );	
		return 0;
	}
	
	/*2.把种子拷贝到torrent目录下面*/
	string cpTorrentCommd = "cp " + store_g + packUuid + ".torrent  " + torrent_g;
	
	Log.WriteLog(LOG_INFO, cpTorrentCommd.c_str() );
	if( Popen( cpTorrentCommd.c_str() ) < 0)
	{
		Log.WriteLog(LOG_INFO, "move torrent failed~~!\n" );
		return -1;
	}
	
	/*异步回调 -> 同步回调 2016-10-27
	iRet = TrnasferResult(TempIp, 8080, argo, true, error);
	if(iRet != 0)
	{
		Log.WriteLog(LOG_ERROR, error.c_str() );	
	}
	*/
	
	/*修改UUID状态为1，下载完成*/
	char *zErrMsg = 0;
	std::stringstream stream;
	stream << "update TORRENT set STATE = 1 where UUID = '" << packUuid << "';";
	
	sqlite3_exec( db_g , stream.str().c_str() , 0 , 0 , &zErrMsg );
	
	return 0;
}


int ftpDownLoad(const string& ftpUrl_, const string& uuid_, const string& destPath_)
{
	int iRet(0);
	int iNum(0);
	
	XMLPlatformUtils::Initialize();

	string sError;
	vector<FileCopyInfo> copyInfo;
	
	string zipComd;
	string Folder = destPath_ + uuid_;
	string temp;	
	string error;
	string uuid_2 = uuid_;
	
	char *zErrMsg = NULL;
	

	if( IsFolderExist( Folder ) )
	{
		if( (0 == HashDcp( destPath_ , uuid_ , sError ))
				&& (0 == access((torrent_g+uuid_+".torrent").c_str(), F_OK)) 
				&& (0 == access((zip_g+uuid_+".zip").c_str(), F_OK))
			)
		{
			temp = Folder + "  already exit! Go next!!!";
			printf("DownLoadDcp OK!\n");
			Log.WriteLog(LOG_INFO, temp.c_str() );
			
			iRet = TrnasferResult(dom_g, 8080, uuid_2, true, error);
			if(iRet != 0)
			{
				Log.WriteLog(LOG_ERROR, error.c_str() );	
			}
			
			/*若dcp包存在且无误，torrrent存在,zip文件存在，则状态置1*/
			std::stringstream stream2;	
			stream2 << "update TORRENT set STATE = 1 where UUID = '" << uuid_2 << "';";
			
			sqlite3_exec( db_g , stream2.str().c_str() , 0 , 0 , &zErrMsg );
			Log.WriteLog(LOG_ERROR, stream2.str().c_str() );	
			
			return 0;	
		}
			
	}
	
	for(iNum = 0; iNum < 3; ++iNum)
	{
		iRet = DownLoadDcp( ftpUrl_ , uuid_ , destPath_ , copyInfo , sError );	
		if( iRet != 0 )
		{
		    Log.WriteLog(LOG_ERROR, "DownLoadDcp error!" );
		    Log.WriteLog(LOG_ERROR, sError.c_str() );	
		        
		    usleep( 10*1000*1000 );
		    continue;
		}
		else
		{
		    printf("DownLoadDcp OK!\n");
		    break;
		}
	}//for
	
	if( 3 == iNum )
	{
		iRet = TrnasferResult(dom_g, 8080, uuid_2, false, error);
		if(iRet != 0)
		{
			Log.WriteLog(LOG_ERROR, error.c_str() );	
		}
		
		std::stringstream stream;	
		stream << "delete from TORRENT where UUID = '" << uuid_2 << "';";
		
		sqlite3_exec( db_g , stream.str().c_str() , 0 , 0 , &zErrMsg );
		Log.WriteLog(LOG_ERROR, stream.str().c_str() );		
		
		return -1;	
	}
	
	iRet = HashDcp( destPath_ , uuid_ , sError );
	
	if( iRet != 0 )
	{
	    Log.WriteLog(LOG_ERROR, "HashDcp error!\n" );
	    return 0;
	}
	else
	{
	    printf("HashDcp OK!\n");
	}

	/* 制作种子并拷贝到torrent目录下面 */	
	iRet = makeToAndIssueFunc( uuid_ );
	if( iRet != 0 )
	{
	    Log.WriteLog(LOG_ERROR, "makeToAndIssueFunc error!\n" );
	    return 0;
	}
	else
	{
	    printf("makeToAndIssueFunc OK!\n");
	}
	
	/* 生成zip压缩文件 */
	zipComd = string("zip -r ") + zip_g + uuid_ + string(".zip ") + string("./") + uuid_;
	
	Log.WriteLog(LOG_INFO, zipComd.c_str() );	
	if( -1 == system_Zip( zipComd.c_str(), store_g.c_str() ) )
	{
		Log.WriteLog(LOG_ERROR, "system_Zip failed!!" );	
		return 0;
	}	

	//异步回调 -> 同步回调 2016-10-27
	iRet = TrnasferResult(dom_g, 8080, uuid_2, true, error);
	if(iRet != 0)
	{
		Log.WriteLog(LOG_ERROR, error.c_str() );	
	}	
		
	return 0;
}

void displayParam(const struct ns1__InterfaceParameter &  param)
{
		printf("**************Begin Dcp Package***************\n");
    printf("%s\n", param.ftpDirName.c_str());
    printf("%s\n", param.ftpIP.c_str());
    printf("%s\n", param.ftpPassWord.c_str());
    printf("%s\n", param.ftpUserName.c_str());
    printf("%s\n", param.uuid.c_str());
    printf("%s\n", param.packageSize.c_str());
    printf("%s\n", param.taskType.c_str());
    printf("%s\n", param.torrentP2pPath.c_str());
    printf("%s\n", param.transferStorePath.c_str());
    printf("%s\n", param.transferTempPath.c_str());
    printf("%s\n", param.transferTorrentPath.c_str());
    printf("**************End Dcp Package***************\n\n");
    
    
    Log.WriteLog(LOG_INFO, "**************Begin Dcp Package***************\n" );
    Log.WriteLog(LOG_INFO, param.ftpDirName.c_str() );
    Log.WriteLog(LOG_INFO, param.ftpIP.c_str() );
    Log.WriteLog(LOG_INFO, param.ftpPassWord.c_str() );
    Log.WriteLog(LOG_INFO, param.ftpUserName.c_str() );
    Log.WriteLog(LOG_INFO, param.uuid.c_str() );
    Log.WriteLog(LOG_INFO, param.packageSize.c_str() );
    Log.WriteLog(LOG_INFO, param.taskType.c_str() );
    Log.WriteLog(LOG_INFO, param.torrentP2pPath.c_str() );
    Log.WriteLog(LOG_INFO, param.transferStorePath.c_str() );
    Log.WriteLog(LOG_INFO, param.transferTempPath.c_str() );
    Log.WriteLog(LOG_INFO, param.transferTorrentPath.c_str() );
    Log.WriteLog(LOG_INFO, "**************End Dcp Package***************\n" );
    
}


/* thread for ns1__maketo */
void* threadForMakeTorrent(void* arg)
{
	pthread_detach(pthread_self());
		
	int iRet(0);	
	string error;
	string uuid_ = (char*)arg;/*Becareful!!*/
	
	string makeTorrent = string("./../BitTorrent-4.0.3/") + "btmaketorrent.py";
	string url = "http://"+tracker_g+":6969/announce";
	string pythonCommd = "python " + makeTorrent + " " + url + " " + store_g + uuid_;
	
	/*制作种子*/
	Log.WriteLog(LOG_INFO, pythonCommd.c_str() );
	if( Popen( pythonCommd.c_str() ) < 0)
	{
	    return 0;
	}
	
	/*把种子拷贝到torrent目录下面*/
	string cpTorrentCommd = "cp " + store_g + uuid_ + ".torrent  " + torrent_g;
	Log.WriteLog(LOG_INFO, cpTorrentCommd.c_str() );
	if( Popen( cpTorrentCommd.c_str() ) < 0)
	{
	    return 0;
	}
	
	/*回调*/
	iRet = TrnasferResult(dom_g, 8080, uuid_, true, error);
	if(iRet != 0)
	{
		Log.WriteLog(LOG_ERROR, error.c_str() );	
	}	
	
	return 0;
}

void* cutTempPackage(void* arg)
{
	int iRet = 0;
	string sError;
	
		
	XMLPlatformUtils::Initialize();
	string _uuid = (char*)arg;
	
	string cpCommd = string("cp -r ") + temp_g + _uuid + " " + store_g;
	string rmCommd = "rm -rf " + temp_g + _uuid;
	
	/*1.把dcp包从临时路径拷贝到存储目录*/
	if( Popen( cpCommd.c_str() ) < 0)
	{
		Log.WriteLog(LOG_ERROR, "cp -r command failed!\n" );
		return 0;
	}
	
	/*2.对存储目录下的dcp包做哈希校验*/
	iRet = HashDcp( store_g , _uuid , sError );
	if( iRet != 0 )
	{
	    printf( "HashDcp error!:%s\n" , sError.c_str() );
	    Log.WriteLog(LOG_ERROR, "HashDcp error!\n" );
	    return 0;
	}
	else
	{
	    printf("HashDcp OK!\n");
	}
	
	/*3.删除临时目录下面的dcp包*/
	if( Popen( rmCommd.c_str() ) < 0)
	{
		Log.WriteLog(LOG_ERROR, rmCommd.c_str() );
	    return 0;
	}
	
	/*4.发布种子*/
	iRet = makeToAndIssueFunc( _uuid );	
	if(iRet < 0)
	{
	    Log.WriteLog(LOG_ERROR, "makeToAndIssue failed!\n" );
	    return 0;
	}
	
	//异步回调 -> 同步回调 2016-10-27
	string error;
	
	iRet = TrnasferResult(dom_g, 8080, _uuid, true, error);
	if(iRet != 0)
	{
		Log.WriteLog(LOG_ERROR, error.c_str() );	
	}	
	
	return 0;
}


/* 工作线程 */
void* ExecuteTransferOrCut( void* arg )
{
		pthread_detach(pthread_self());
		
		int tRet(0);
		
		while(true)
		{
			int nrow          = 0;  
			int ncolumn       = 0; 
			char **azResult   = NULL; 
			char *zErrMsg     = NULL;  
			struct hostent *h = NULL; 
			const char* sql   = "select UUID,FTPUSERNAME,FTPPASSWORD,FTPIP,FTPDIRNAME,TASKTYPE \
					from TORRENT where STATE = 0;"; 
			
	 
			sqlite3_get_table( db_g , sql , &azResult , &nrow , &ncolumn , &zErrMsg );  
			
			for(int i = 1; i <= nrow; ++i)
			{							
				string uuid_t        = azResult[i*ncolumn + 0];
				string ftpUserName_t = azResult[i*ncolumn + 1];
				string ftpPassWord_t = azResult[i*ncolumn + 2];
				string ftpIP_t       = azResult[i*ncolumn + 3];
				string ftpDirName_t  = azResult[i*ncolumn + 4];
				string taskType_t    = azResult[i*ncolumn + 5];
						              
				if(0 == strcmp(ftpIP_t.c_str(), "mrs.oristartech.cn"))
					ftpIP_t = "119.10.114.98";
			
				if( ContainAlpha(ftpIP_t) )
				{
				   if( ( h = gethostbyname( ftpIP_t.c_str() ) ) != NULL )
				   {
				        ftpIP_t = inet_ntoa(*((struct in_addr *)h->h_addr));
				   }
				   else
				   {
				      Log.WriteLog(LOG_ERROR, "gethostbyname error!" );
				   }
				}   
				
				string sFTP_URL = "ftp://" + ftpUserName_t + ":" + ftpPassWord_t
				      + "@" + ftpIP_t + "/" + ftpDirName_t + "/" + uuid_t + "/"; 
				
				Log.WriteLog(LOG_INFO, sFTP_URL.c_str() );           			
				
				if( 0 == strcasecmp(taskType_t.c_str(), "transfer") )
				{				
						tRet = ftpDownLoad( sFTP_URL, uuid_t, store_g );				 		
				}
				else if( 0 == strcasecmp(taskType_t.c_str(), "cut") )
				{	
						cutTempPackage( (void*)uuid_t.c_str() );					
				}
				else
				{
				    Log.WriteLog(LOG_ERROR, taskType_t.c_str() );  
				}
			}
			
			sqlite3_free_table( azResult );
			
			if(0 == nrow)
			{
				//Log.WriteLog(LOG_INFO, "There is no data! Sleep 1 Minute!" ); 
				usleep(60*1000*1000);
			}	
		}//while(true)
		
		return 0;
}


int ns1__isPackExist(struct soap*, struct ns1__InterfaceParameter param, bool &ret)
{
	DIR *dir;
	struct dirent *ptr;
	ret = false;
	
	if ( ( dir = opendir( store_g.c_str() ) ) == NULL)
	{
	    Log.WriteLog(LOG_ERROR, "opendir error!" );
	    Log.WriteLog(LOG_ERROR, store_g.c_str() );
	    ret = false;
	    return 0;
	}
	
	while ((ptr = readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
	        continue;
	
	    if( (ptr->d_type == 4) && (strcmp(ptr->d_name, param.uuid.c_str()) == 0) )
	    {
	        ret = true;
	    }
	}
	
	if( closedir(dir) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "closedir failed!" );
		ret = false;
		return 0;
	}
	
	return 0;
}

int ns1__transfer(struct soap*, struct ns1__InterfaceParameter param, bool &ret)
{
	
	string sFTP_URL;
	string destPath;
	string pklUuid;
	
	char* zErrMsg = 0;
	
	displayParam( param );
	
	if((param.ftpUserName == "") || (param.ftpPassWord == "") || (param.ftpIP == "") ||
			(param.ftpDirName == "") || (param.uuid == "") || (param.taskType == "") )
	{
		Log.WriteLog(LOG_ERROR, "param is null!" );
		ret = false;		
		return 0;
	}
	
	/*
	pthread_mutex_lock( &mutex_ );
	Que.push( param );
	pthread_mutex_unlock( &mutex_ );
	*/
	
	std::stringstream stream;		
	stream << "INSERT INTO TORRENT VALUES('";
	stream << param.uuid << "','";
	stream << param.ftpUserName << "','";
	stream << param.ftpIP << "','";
	stream << param.ftpPassWord << "','";
	stream << param.ftpDirName << "','";
	stream << param.torrentP2pPath << "','";
	stream << param.packageName << "','";
	stream << param.packageSize << "','";
	stream << param.taskType << "','";
	stream << param.transferTempPath << "','";
	stream << param.transferStorePath << "','";
	stream << param.transferTorrentPath << "',0);";	
	
	sqlite3_exec( db_g , stream.str().c_str() , 0 , 0 , &zErrMsg );
	Log.WriteLog(LOG_INFO, stream.str().c_str() );
	
	ret = true;
	return 0;	
}

/*Tomorrow~~~~~~~~~~~~!*/
int ns1__cutPackage(struct soap*, struct ns1__InterfaceParameter param, bool &ret)
{
		int iRet = 0;
		pthread_t pid;
		
		char uuid_[64];
		memset(uuid_, 0, 64);
		
		strcpy( uuid_, param.uuid.c_str() );
		
		iRet = pthread_create(&pid, NULL, cutTempPackage, (void*)uuid_);
  	
		if( iRet != 0 )
		{
			printf("pthread_create cutTempPackage failed!");
			Log.WriteLog(LOG_INFO, "pthread_create cutTempPackage failed!" );
			ret = false;
			return 0;
		}
		
		ret = true;
		return 0;
}

int ns1__getUsableSpace(struct soap*, struct ns1__InterfaceParameter param, std::string &ret)
{
	struct statfs diskInfo;
	
	char sTotal[100];	
	memset( sTotal,'\0',sizeof(sTotal) );
	
	statfs( store_g.c_str(), &diskInfo );
	
	unsigned long long blocksize = diskInfo.f_bsize;
	unsigned long long totalsize = blocksize * diskInfo.f_blocks;
	unsigned long long Total = totalsize>>10;
	
	sprintf( sTotal, "%llu", Total );
	
	Log.WriteLog(LOG_INFO, sTotal );
	
	ret = sTotal;
	
	return 0;
}

int ns1__deletePackage(struct soap*, struct ns1__InterfaceParameter param, bool &ret)
{
	ret = true;

	string file_torrent = torrent_g + param.uuid + ".torrent";
	string file_store   = store_g + param.uuid + ".torrent" ;
	string file_zip     = zip_g + param.uuid + ".zip";
	string uuid_store   = store_g + param.uuid;
	string delCmd       = "delete UUID " + param.uuid;
	
	Log.WriteLog(LOG_INFO, delCmd.c_str());
	
	if( remove( file_torrent.c_str() ) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "remove error!" );	
		ret = false;
	}	
	if( remove( file_store.c_str() ) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "remove error!" );	
		ret = false;
	}	
	if( remove( file_zip.c_str() ) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "remove error!" );	
		ret = false;
	}
	
	remove_directory( uuid_store.c_str() );
	
	return 0;
}


int ns1__getTempPackage(struct soap*, struct ns1__InterfaceParameter param, std::vector<std::string >&ret)
{
	DIR *dir;
	struct dirent *ptr;
	
	DIR *dir2;
	struct dirent *ptr2;
	struct stat buf;
	
	string packageName;
	string packageSize;
	string packageUUID;
	
	/*Begin init XMLPlatform*/
	try {
	  XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		printf("Initialize Error!\n");
	  return 1;
	}
	
	XercesDOMParser* parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);    // optional
	
	
	ret.clear();
	
	/*获取配置文件路径*/
	char	Temp[200];	
	char childPath[512];
	string tempPath;
	
	memset(Temp, 0, 200);
	
	if ( GetProfileString( "./downloadPath.ini", "p2pServerPath", "Path_tempPath", Temp ) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "GetProfileString from downloadPath.ini failed!" );	
	}

	
	
	tempPath = Temp;
	
	if ((dir = opendir( tempPath.c_str() )) == NULL)
	{
			Log.WriteLog(LOG_INFO, "Open dir error...\n" );
			Log.WriteLog(LOG_INFO, tempPath.c_str() );
	    printf("Open dir error...\n");
	    return 0;
	}
	
	while ((ptr = readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
	        continue;
	
	    if(ptr->d_type == 4)
	    {
				packageName = ptr->d_name;
				long long fileSize = 0;
				memset(childPath, 0, 512);
				sprintf( childPath, "%s%s/", tempPath.c_str(), ptr->d_name );
				
				if ((dir2 = opendir( childPath )) == NULL)
				{
					Log.WriteLog(LOG_INFO, "Open dir error...\n" );
					return 0;
				}
				while ((ptr2 = readdir(dir2)) != NULL)
				{
					if(strcmp(ptr2->d_name,".")==0 || strcmp(ptr2->d_name,"..")==0)
						continue;
					
					string fileName = string(childPath) + ptr2->d_name;
					stat(fileName.c_str(), &buf);
					fileSize += buf.st_size;
					
					char* pFile = Letter_strlwr( ptr2->d_name );
					if( strstr(pFile, ".xml") != NULL )
					{
						parser->parse(fileName.c_str());
						
						
						//读取
						DOMDocument* doc = parser->getDocument();
						DOMNodeList* nodelist = doc->getChildNodes();
						DOMNode* root;
						DOMNodeList* childlist;
						DOMNode* child;
      	
						
						for (unsigned int i = 0; i < nodelist->getLength(); i++)
						{
							root = nodelist->item(i);
							char *nodename = XMLString::transcode(root->getNodeName());
						
							if( strstr(nodename, "PackingList") == NULL )
							{
								XMLString::release(&nodename);
								continue;
							}
							
							
							XMLString::release(&nodename);
						
							if (root->hasChildNodes())
							{
								childlist = root->getChildNodes();
						
								for (unsigned int j = 1; j < childlist->getLength()-1; j+=2)
								{
									child = childlist->item(j);
									char *childname = XMLString::transcode(child->getNodeName());
						
									if( strstr(childname, "Id") != NULL)
									{
										char* textContent = XMLString::transcode(child->getTextContent());
      	
										string childname_ = textContent;
										childname_.erase(0, 9);
										
										packageUUID = childname_;
								
										XMLString::release(&textContent);
										XMLString::release(&childname);
										break;
									}
							
									XMLString::release(&childname);
								}
							}
						  
						}
											
					}
					
					
				}
				
				packageSize = ltos(fileSize);
				
				
				/*Begin to json*/
				std::stringstream stream;
    		stream << "{\"" << "packageName" << "\":\"";
	    	
    		stream << packageName;
				stream << "\",\"";
				
				stream << "packageSize" << "\":\"";
				stream << packageSize;
				stream << "\",\"";
				
				stream << "packageUUID" << "\":\"";
				stream << packageUUID;
	    	
				stream << "\"}";
				
				ret.push_back( stream.str() );
				/*End to json*/
	    }
	}
	
	if( closedir(dir) < 0 )
	{
		Log.WriteLog(LOG_ERROR, "closeDir failed!" );
	}	
	
	delete parser;	
	
	XMLPlatformUtils::Terminate();
	
	return 0;
}

int ns1__maketo(struct soap*, struct ns1__InterfaceParameter param, bool &ret)
{
		int iRet = 0;
		pthread_t pid;
		
		char uuid_[1024];
		memset(uuid_, 0, 1024);
		
		strcpy( uuid_, param.uuid.c_str() );
		
		iRet = pthread_create(&pid, NULL, threadForMakeTorrent, (void*)uuid_);
  	
		if( iRet != 0 )
		{
			printf("pthread_create threadForMakeTorrent failed~~!\n");
			Log.WriteLog(LOG_ERROR, "pthread_create threadForMakeTorrent failed!" );
			ret = false;
			return 0;
		}
		
		ret = true;
		return 0;
}



int TrnasferResult(const std::string &ip, unsigned short port, std::string &arg0, bool arg1, std::string &error)
{
	char buffer[bufferSize];
	memset(buffer, 0, bufferSize);
	sprintf(buffer, "http://%s:%d/dom/services/domService", ip.c_str(), (int)port);
	
	Log.WriteLog(LOG_INFO, buffer );

	struct soap csoap;
	soap_init(&csoap);

	ns21__TransferResultSecond request;
	ns21__TransferResultSecondResponse response;
	request.arg0 = &arg0;
	request.arg1 = arg1;

	int result = soap_call___ns21__TransferResultSecond(&csoap, buffer, NULL, &request, &response);
	if(result != SOAP_OK)
	{
		error = *soap_faultstring(&csoap);
		return 0;
	}

	soap_destroy(&csoap);
	soap_end(&csoap);
	soap_done(&csoap);
	return 0;
}

//不用关注如下函数
int __ns21__transferResult(struct soap*, ns21__transferResult *ns21__transferResult_, ns21__transferResultResponse *ns21__transferResultResponse_){return 0;}
int __ns21__TransferResultSecond(struct soap*, ns21__TransferResultSecond *ns21__TransferResultSecond_, ns21__TransferResultSecondResponse *ns21__TransferResultSecondResponse_){return 0;}
int __ns21__getSendinfo(struct soap*, ns21__getSendinfo *ns21__getSendinfo_, ns21__getSendinfoResponse *ns21__getSendinfoResponse_){return 0;}
int __ns21__dispense(struct soap*, ns21__dispense *ns21__dispense_, ns21__dispenseResponse *ns21__dispenseResponse_){return 0;}
int __ns21__addNode(struct soap*, ns21__addNode *ns21__addNode_, ns21__addNodeResponse *ns21__addNodeResponse_){return 0;}
int __ns21__addSendinfo(struct soap*, ns21__addSendinfo *ns21__addSendinfo_, ns21__addSendinfoResponse *ns21__addSendinfoResponse_){return 0;}
