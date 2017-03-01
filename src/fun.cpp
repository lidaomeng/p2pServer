/*
*@file:fun.cpp
*@brief:工具函数库
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __FUN_H__
#include "fun.h"
#endif

int searchFile(const char *pathname, const char *filename, char **filelist, int *nfile,int max)
{
    DIR *dir;
    struct dirent *dirt;
    char tmppath[PATH_MAX] = {0}; 
    //char pathname2[PATH_MAX] = {0}; 
    int ret =0;

    if(pathname==NULL || filename==NULL || nfile==NULL){printf("para is NULL!\n"); return -1;}// 
    //printf("%s\n",pathname);
    dir = opendir(pathname);
    if(dir==NULL)
    {
        //if(errno!=ENOENT && *nfile>0) 
        //if(*nfile>0) 
        //    while(--(*nfile)>=0) free(filelist[*nfile]);
        //printf("%s\n",strerror(errno));
        return errno;
    }
    while((dirt=readdir(dir))!=NULL)
    {
        //printf("%s\n",dirt->d_name);
        if(strcmp(dirt->d_name,".")==0 ||strcmp(dirt->d_name,"..")==0)
            continue;
#ifndef _DIRENT_HAVE_D_TYPE
        printf("_DIRENT_HAVE_D_TYPE is not defined!\n");
        closedir(dir);
        return -1;
#endif

		bool bIsDIR = false;
		char tmpPathname[PATH_MAX] = {0}; 
		sprintf(tmpPathname,DIR_FMTSTR,pathname,dirt->d_name);
		struct stat tmpst;
		if( stat( tmpPathname , &tmpst ) < 0 )
		{
			printf("stat %s is error.\n" , tmpPathname );
			return -1;
		}
		else if ( S_ISDIR(tmpst.st_mode) )
		{
			bIsDIR = true;
			printf("this is directory.\n");
		}

        if( dirt->d_type==DT_DIR  || bIsDIR )
        {
            //sprintf(tmppath,"%s/%s",pathname,dirt->d_name);
			sprintf(tmppath,DIR_FMTSTR,pathname,dirt->d_name);
            ret = searchFile(tmppath,filename,filelist,nfile,max);
            if(ret==-2)
            {
                closedir(dir);
                return -2;
            }
#if 0
            if(ret!=0)
            {
                if(ret==ENOENT) continue;
                closedir(dir);
                if(*nfile>0) while(--(*nfile)>=0) free(filelist[*nfile]);
                //printf("%s\n",strerror(errno));
                return ret;
            }
#endif
        }
		else
		{
            if(strcmp(dirt->d_name,filename)==0)
            {
                if(*nfile>=max)
                {
                    printf("nfile=%d is bigger than max!\n",*nfile);closedir(dir);
                    //if(*nfile>0) while(--(*nfile)>=0) free(filelist[*nfile]);
                    return -2;
                }
                filelist[*nfile] = (char *)malloc(PATH_MAX);
                //sprintf(filelist[*nfile],"%s/%s",pathname,filename);
				sprintf(filelist[*nfile],DIR_FMTSTR,pathname,filename);
                //printf("%s,%d\n",filelist[*nfile],*nfile);
                (*nfile)++;
            }
        }
    }
    closedir(dir);
    return 0;
}
int searchFile2(const char *pathname,const char *filename,char **filelist,int *nfile,int max)
{
	// the blurring search, e.g "*.xml"
    DIR *dir;
    struct dirent *dirt;
    char tmppath[PATH_MAX] = {0}; 
    int ret =0;

    if(pathname==NULL || filename==NULL || nfile==NULL){printf("para is NULL!\n"); return -1;}// 

    char srcPath[PATH_MAX] = {0};
    strcpy(srcPath,pathname);
    //if(srcPath[strlen(srcPath)-1]=='/')
	if( srcPath[strlen(srcPath)-1]== DIR_SEPCHAR )
        //strncpy(pathName,pathname,pathname.strlen()-1);
        srcPath[strlen(srcPath)-1] = '\0';
    dir = opendir(srcPath);
    if(dir==NULL)
    {
        //if(errno!=ENOENT && *nfile>0) 
        //if(*nfile>0) 
        //    while(--(*nfile)>=0) free(filelist[*nfile]);
        return errno;
    }
    while((dirt=readdir(dir))!=NULL)
    {
        //printf("%s\n",dirt->d_name);
        if(strcmp(dirt->d_name,".")==0 ||strcmp(dirt->d_name,"..")==0)
            continue;
#ifndef _DIRENT_HAVE_D_TYPE
        printf("_DIRENT_HAVE_D_TYPE is not defined!\n");
        closedir(dir);
        return -1;
#endif
		
		bool bIsDIR = false;
		char tmpPathname[PATH_MAX] = {0}; 
		sprintf(tmpPathname,DIR_FMTSTR,pathname,dirt->d_name);
		struct stat tmpst;
		if( stat( tmpPathname , &tmpst ) < 0 )
		{
			printf("stat %s is error.\n" , tmpPathname );
			return -1;
		}
		else if ( S_ISDIR(tmpst.st_mode) )
		{
			bIsDIR = true;
			printf("this is directory.\n");
		}

        if( dirt->d_type==DT_DIR || bIsDIR )
        {
            //sprintf(tmppath,"%s/%s",srcPath,dirt->d_name);
			sprintf(tmppath,DIR_FMTSTR,srcPath,dirt->d_name);
            ret = searchFile2(tmppath,filename,filelist,nfile,max);
            if(ret==-2) // arrived at the number Max
            {
                closedir(dir);
                return -2;
            }
#if 0
            if(ret!=0)
            {
                if(ret==ENOENT) continue;
                closedir(dir);
                if(*nfile>0) while(--(*nfile)>=0) free(filelist[*nfile]);
                return ret;
            }
#endif
        }else{
            if(strstr(dirt->d_name,filename)!=NULL)
            {
                //printf("%s\n",dirt->d_name);
                if(*nfile>=max) 
                {
                    printf("nfile=%d is bigger than max\n",*nfile);closedir(dir);
                    //if(*nfile>0) while(--(*nfile)>=0) free(filelist[*nfile]);
                    return -2;
                }
                char *ptmp = (char *)malloc(PATH_MAX);
                if(ptmp==NULL) return -3;
                filelist[*nfile] = ptmp; 
                //sprintf(filelist[*nfile],"%s/%s",srcPath,dirt->d_name);
				sprintf(filelist[*nfile],DIR_FMTSTR,srcPath,dirt->d_name);
                //printf("%s,%d\n",filelist[*nfile],*nfile);
                (*nfile)++;
            }
        }
    }
    closedir(dir);
    return 0;
}

int MakeDirectory( const std::string DestDir )
{
	int ret(0);

	std::string  Dest = DestDir;
	if ( Dest.length()==0 )
	{
		errno = -2;
		return errno;
	}
	//check file directory; 如果没有dest目录，则建立该路径。
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
			ret = system( (MKDIR + Dest).c_str() );
			if ( ret == 0 )
			{
#ifdef _TEST_
				printf( "File Directory Created :%s\n", Dest.c_str() );
#endif
			}
			else
			{
#ifdef _TEST_
				printf( "File Directory Create Fail :%s\n", Dest.c_str() );
#endif
				return errno;
			}
			errno = 0;
		}
		else
		{
#ifdef _TEST_
			perror("\nUnable to delete directory\n");
#endif
		}
	}
	else
	{
		//如果没有dest目录，则建立该路径。
		//ret = mkdir(Dest.c_str());
		ret = system( (MKDIR + Dest).c_str() );
		if ( ret == 0 )
		{
#ifdef _TEST_
			printf( "File Directory Created :%s\n", Dest.c_str() );
#endif
		}
		else
		{
#ifdef _TEST_
			printf( "File Directory Create Fail :%s\n", Dest.c_str() );
#endif
			return errno;
		}
	}

	return ret;
}

int MakeDirectory2( const std::string DestDir )
{
	int ret(0);

	std::string  Dest = DestDir;
	if ( Dest.length()==0 )
	{
		errno = -2;
		return errno;
	}
	//check file directory; 如果没有dest目录，则建立该路径。
	if( ( ret = access( Dest.c_str() , F_OK ) ) == 0 )     
	{
#ifdef _TEST_
		printf( "File Directory already exists :%s\n", Dest.c_str() );
#endif
		
	}
	else
	{
		//如果没有dest目录，则建立该路径。
		//ret = mkdir(Dest.c_str());
		ret = system( (MKDIR + Dest).c_str() );
		if ( ret == 0 )
		{
#ifdef _TEST_
			printf( "File Directory Created :%s\n", Dest.c_str() );
#endif
		}
		else
		{
#ifdef _TEST_
			printf( "File Directory Create Fail :%s\n", Dest.c_str() );
#endif
			return errno;
		}
	}

	return ret;
}

int RemoveDir( const std::string DestDir )
{
	int ret(0);

	std::string  Dest = DestDir;
	if ( Dest.length()==0 )
	{
		errno = -2;
		return errno;
	}

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
			
		}
	}

	return ret;
}

int PreRemoveDir( const std::string DestDir )
{
	int ret(0);

	std::string  Dest = DestDir;
	if ( Dest.length()==0 )
	{
		errno = -2;
		return errno;
	}

	if( ( ret = access( Dest.c_str() , F_OK ) ) == 0 )     
	{
#ifdef _TEST_
		printf( "File Directory already exists :%s\n", Dest.c_str() );
#endif
		//ret = system( (RMDIR + Dest).c_str() );
		std::string dirName;
		dirName = Dest;
		DIR *dir = opendir(dirName.c_str());
		dirent *dirFile;
		while((dirFile = readdir(dir)) != NULL)
		{
			if(strcmp(dirFile->d_name,".")==0 ||strcmp(dirFile->d_name,"..")==0)
				continue;

			if(dirFile->d_type==DT_DIR)
			{
				;
			}
			else
			{
				//获得文件大小
				unsigned long long Length = 0;
				struct stat stsrc;
				if( lstat( dirFile->d_name , &stsrc ) == 0 )
					Length = stsrc.st_size;
				else{
#ifdef _TEST_
					printf("lstat %s is err:%s!\n",dirFile->d_name , strerror(errno));
#endif
					closedir(dir);
					return -2;
				}
				//比较大小，大于100MB的清空
				if ( Length >= (100*1024*1024) )
				{
					std::string sCmd = "cat /dev/null > ";
					sCmd += (dirFile->d_name) ;
					ret = system( sCmd.c_str() );
					if( ret == 0 )
					{
						printf( "\n exec finished:%s\n", sCmd.c_str() );
						errno = 0;
					}
					else
					{
						printf( "\n exec fail:%s\n", sCmd.c_str() );
					}
				}
			}
		}
		closedir(dir);

	}

	return ret;
}
//---zhangmiao:begin---[7/10/2012]----
int listFilesOnFTP( std::string sFTP_URL ,std::vector<cftp::FileInformation> &fileList )   
{
	cftp::FtpClient m_ftp;
	//log in
	int result = m_ftp.LogIn(sFTP_URL);
	if(result != cftp::NOANYERROR)
		return result;
	//get file name from ftp server in current direction
	result = m_ftp.ListFile("", fileList);
	if(result != cftp::NOANYERROR)
		return result;

	return 0;
}
int listFilesOnFTP( cftp::FtpClient& rftp ,std::string dir_address ,std::vector<cftp::FileInformation> &fileList ,std::string filename )
{ 
	std::vector<cftp::FileInformation> vfileList;
	int result = rftp.ListFile(dir_address, vfileList);
	if(result != cftp::NOANYERROR)
		return result;
	if (vfileList.size()==0)
	{
		return 100;
	}
	for (size_t i=0;i<vfileList.size();i++)
	{
		cftp::FileInformation FileInfo = vfileList[i];
		std::string type = FileInfo.type; //获取文件属性      
		std::string size = FileInfo.size; //获取文件大小     
		std::string name = FileInfo.name; //获取文件名      
		std::string address = dir_address + name; //文件的完整地址
		if( type[0] == 'd') //如果该文件是目录 
		{
			std::vector<cftp::FileInformation> tmp;
			int ret = listFilesOnFTP(rftp, dir_address+name+'/',tmp , filename ); //递归 
			if( ret == 0 )  
				fileList.insert(fileList.end(), tmp.begin(),tmp.end());
		}
		else //如果不是目录，逐项添加到数组 
		{
			if(filename.size())
			{
				size_t pos = address.rfind(filename);
				if( pos != std::string::npos )
				{
					FileInfo.name = address;
					fileList.push_back(FileInfo);
				}
				else
				{
					continue;
				}
			}
			else
			{
				FileInfo.name = address;
				fileList.push_back(FileInfo);
			}
		}
    
	}

	return result;
}

int listFilesOnFTPWithUrl( std::string sFTP_URL ,std::string dir_address ,std::vector<cftp::FileInformation> &fileList ,std::string filename ) 
{
	cftp::FtpClient m_ftp;
	//log in
	int result = m_ftp.LogIn(sFTP_URL);
	if(result != cftp::NOANYERROR)
		return result;

	result = listFilesOnFTP( m_ftp, dir_address ,fileList ,filename );
	if(result != cftp::NOANYERROR)
		return result;

	return 0;
}
//---zhangmiao:end--[7/10/2012]--
int Remove_PrefixOfUuid( std::string& rUuid )
{
	size_t pos;
	if ( ( pos = rUuid.find("urn:uuid:") ) != rUuid.npos && pos==0 )
	{
		rUuid = rUuid.substr(9);
		return 0;
	}
	else{
		return 1;
	}
}


#define _CONVERT_TIMEZONEINFO_

//@author zhangmiao@oristartech.com
//@date 2014-10-08
//@brief 增加新的时区处理代码，sDestTZ='+08:00'为北京时间，如:sSrcTimeStr="2009-07-04T10:00:00+09:00"，转换为sDestTimeStr="2009-07-04 09:00:00"；
//@new
int ConvertTimeZoneInfo( std::string sSrcTimeStr , std::string sDestTZ , std::string& sDestTimeStr )
{
	//时间日期函数更换 2014-12-11
#ifdef _CONVERT_TIMEZONEINFO_
	int ret = 0;
	int nYear=0,nMon=0,nDay=0,nHour=0,nMin=0,nSec=0,nTz_Hour=0,nTz_Min=0;
	//"2014-10-01T12:00:00+09:00";
	sscanf(sSrcTimeStr.c_str(),"%04d-%02d-%02dT%02d:%02d:%02d%d:%02d", &nYear , &nMon , &nDay , &nHour , &nMin , &nSec ,&nTz_Hour , &nTz_Min );
	struct tm *local=NULL , tmSrcTime;
	time_t timeVal = 0;

	tmSrcTime.tm_sec  = nSec;
	tmSrcTime.tm_min  = nMin;
	tmSrcTime.tm_hour = nHour;
	tmSrcTime.tm_mday = nDay;
	tmSrcTime.tm_mon  = nMon  - 1;
	tmSrcTime.tm_year = nYear - 1900;

	char sTZ_Env[50]="",sDestTZ_Env[50]="";
	sprintf(sTZ_Env , "TZ=PST%+03d:%02d" , -nTz_Hour , nTz_Min );
	//putenv("TZ=PST-8");
	putenv( sTZ_Env );
	tzset();

	timeVal = mktime( &tmSrcTime );

	int nDestTz_Hour=8, nDestTz_Min=0;

	if ( sDestTZ.length()==0 )
	{
		putenv( (char*)"TZ=PST-08:00" );
	}
	else
	{
		nDestTz_Hour = 8 , nDestTz_Min = 0;
		sscanf(sDestTZ.c_str(),"%d:%02d", &nDestTz_Hour , &nDestTz_Min );
		sprintf(sDestTZ_Env , "TZ=PST%+03d:%02d" , -nDestTz_Hour , nDestTz_Min );
		putenv( sDestTZ_Env );
	}

	tzset();

	local = localtime( &timeVal );
	//local = gmtime(&td);
	printf("Current time = %s\n", asctime(local));

	putenv((char*)"TZ=");
	tzset();

	char sRetTimeStr[50]="";
	sprintf( sRetTimeStr , "%04d-%02d-%02d %02d:%02d:%02d" , local->tm_year + 1900 ,
		local->tm_mon + 1 ,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec );

	sDestTimeStr = sRetTimeStr;

	return ret;

#else

	int ret = 0;
	C_Time tmptime;
	ret = tmptime.setTimeTStr( sSrcTimeStr );

	tmptime.getTimeStrDb( sDestTimeStr );
	return 0;
#endif

}
//@modify(时区处理)end;
