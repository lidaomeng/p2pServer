#include "soapserverprocess.h"
#include "LogManage.h"
#include "config.h"
#include <queue>
#include <map>
#include <string>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C"
{
	#include "sqlite3.h"
}


LogManage Log( LOG_INFO, 10*1024*1024, "./../log/", "p2p-Server" );
extern void* ExecuteTransferOrCut( void* arg );


std::string store_g;
std::string temp_g;
std::string torrent_g;
std::string zip_g;
std::string tracker_g;
std::string dom_g;
	
sqlite3 *db_g = NULL;


int main()
{
	short port = 12330;
	int 	iRet = 0;
	pthread_t tid;
	
	int rc;
	char *zErrMsg = NULL; 
	struct hostent *host = NULL; 
	
	try
	{
		/*begin 动态申请数组*/
		char* store_tmp = new char[64];
		char* temp_tmp = new char[64];
		char* torrent_tmp = new char[64];
		char* zip_tmp = new char[64];
		char* tracker_tmp = new char[64];
		char* dom_tmp = new char[64];
		
		bzero(store_tmp, 64);
		bzero(temp_tmp, 64);
		bzero(torrent_tmp, 64);
		bzero(zip_tmp, 64);
		bzero(tracker_tmp, 64);
		bzero(dom_tmp, 64);
		
		if ( GetProfileString( "./downloadPath.ini", "p2pServerPath", "Path_storePath", store_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		if ( GetProfileString( "./downloadPath.ini", "p2pServerPath", "Path_tempPath", temp_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		if ( GetProfileString( "./downloadPath.ini", "p2pServerPath", "Path_torrent", torrent_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		if ( GetProfileString( "./downloadPath.ini", "p2pServerPath", "Path_zipPath", zip_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		if ( GetProfileString( "./downloadPath.ini", "TrackIp", "Track_ip", tracker_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		if ( GetProfileString( "./downloadPath.ini", "DomIp", "Dom_Ip", dom_tmp ) < 0 )
		{
			printf("GetProfileString from downloadPath.ini failed!\n");
			return -1;	
		}
		
		std::string dom = dom_tmp;
		if( ( host = gethostbyname( dom.c_str() ) ) != NULL )
		{
		     dom = inet_ntoa(*((struct in_addr *)host->h_addr));
		}
		else
		{
			printf("gethostbyname error!\n");
		}
		
		store_g   = store_tmp;
		temp_g    = temp_tmp;
		torrent_g = torrent_tmp;
		zip_g     = zip_tmp;
		tracker_g = tracker_tmp;
		dom_g     = dom;
		
		delete [] store_tmp;
		delete [] temp_tmp;
		delete [] torrent_tmp;
		delete [] zip_tmp;
		delete [] tracker_tmp;
		delete [] dom_tmp;
		
		Log.WriteLog(LOG_INFO, "lidaomeng123" );
		Log.WriteLog(LOG_INFO, dom_g.c_str() );
		
		/*end 动态申请数组*/
	}
	catch(...)
	{
		abort();	
	}
	
	rc = sqlite3_open("sqlite.db", &db_g);
	if( rc )   
	{   
	 fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_g));   
	 sqlite3_close(db_g);   
	 return 1;   
	}
	
  const char* sql = "CREATE TABLE TORRENT (\
  	UUID VARCHAR(50) PRIMARY KEY,\
  	FTPUSERNAME VARCHAR(50),\
  	FTPIP VARCHAR(50),\
  	FTPPASSWORD VARCHAR(50),\
  	FTPDIRNAME VARCHAR(50),\
  	TORRENTP2PPATH VARCHAR(50),\
  	PACKAGENAME VARCHAR(50),\
  	PACKAGESIZE VARCHAR(50),\
  	TASKTYPE VARCHAR(50),\
  	TRANSFERTEMPPATH VARCHAR(50),\
  	TRANSFERSTOREPATH VARCHAR(50),\
  	TRANSFERTORRENTPATH VARCHAR(50),\
  	STATE INTEGER);";
  	   
	sqlite3_exec( db_g , sql , 0 , 0 , &zErrMsg );    
	
	iRet = pthread_create( &tid, NULL, ExecuteTransferOrCut, NULL );  	
	if( iRet != 0 )
	{
		printf("pthread_create cutTempPackage failed~~!\n");
		return 1;
	}	


	SoapServerProcess((void*)(&port));

	sqlite3_close(db_g);
	return 0;
}
