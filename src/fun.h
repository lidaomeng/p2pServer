/*
*@file:fun.h
*@brief:���ߺ�����
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __FUN_H__
#define __FUN_H__

//#undef _FILE_OFFSET_BITS
//#define _FILE_OFFSET_BITS 64
#include <string>
#include <vector>
#include "errdef.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>

#include <dirent.h>
#include <time.h>

#ifdef WIN32
#include <io.h>
#define DIR_SEPCHAR '\\'
#define DIR_FMTSTR "%s\\%s"

#include <limits.h>
#define PATH_MAX            512
#define NAME_MAX 255
#include <direct.h>
#else
#define DIR_SEPCHAR '/'
#define DIR_FMTSTR "%s/%s"
#endif

//#include "C_Time.h"

#ifdef WIN32
//#define atoll atol
#define atoll atof
//#define lstat stat
#define lstat _stat64
#define stat  _stat64
#define MKDIR "mkdir  "
#define RMDIR "rmdir /q/s "
#else
#define MKDIR "mkdir -p "
#define RMDIR "rm -rf "
#endif
#ifdef WIN32
#define	F_OK	0
#define	R_OK	4
#define	W_OK	2
#define	X_OK	1
#endif

#ifdef __cplusplus 
extern "C" {
#endif

#define BUF_SIZE 1024
#define BSIZE 8192 

/************************************************* 
*@brief��  �����ļ������ļ���filename
*@param��  pathname �������������ļ�·����������"/"��β�� :[in]
*@param��  filename ��Ҫ�������ļ���                  :[in]
*@param��  filelist�� �������������ļ���·������1��2ά����         ��out��
*@param��  max�� ������������ļ���         ��in��
*@param��  nfile �� �������������ļ�������   :[out]
*@Return�� ������,0 = �ɹ���-1=����:PKL�ļ���û���ҵ�.
*************************************************/
//�����ļ������ļ���filename.
int searchFile(const char *pathname, const char *filename, char **filelist, int *nfile, int max);
//�����ļ������ļ���չ�����磺.xml
int searchFile2(const char *pathname, const char *filename, char **filelist, int *nfile, int max);
//����Ŀ¼
int MakeDirectory( const std::string DestDir );
//����Ŀ¼�����Ŀ��Ŀ¼������ɾ����
int MakeDirectory2( const std::string DestDir );
//ɾ��Ŀ¼
int RemoveDir( const std::string DestDir );
int PreRemoveDir( const std::string DestDir );

int Remove_PrefixOfUuid( std::string& rUuid );

#ifdef __cplusplus
}
#endif
//---zhangmiao:begin---[7/10/2012]----
#include "FtpClient.h"
/*
 * @brief������: ����ftp�������ϵ������ļ���linux�����е�ǰĿ¼
 * @param��  sFTP_URL: ָ����FTP URL                             : [in]
 * @param��  fileList: �������������ļ���[FileInformation]�б�   :��out��
 * @Return������ֵ: ������,0 = �ɹ���-1=����:û���ҵ�.
 */
//list ��ǰĿ¼
int listFilesOnFTP( std::string sFTP_URL ,std::vector<cftp::FileInformation> &fileList );
/************************************************* 
*@brief��  ����ftp��������ָ��Ŀ¼�µ������ļ�
*@param��  rftp: ftp����                                        :[in]
*@param��  dir_address: Ҫ������Ŀ¼                            :[in]
*@param��  filelist�� �������������ļ���[FileInformation]�б�   :��out��
*@param��  filename�� Ҫ�������ļ���                            :��in��
*@Return�� ������,0 = �ɹ���-1=����:û���ҵ�.
*************************************************/
//list �����ļ�
int listFilesOnFTP( cftp::FtpClient& rftp ,std::string dir_address ,std::vector<cftp::FileInformation> &fileList ,std::string filename="" );
/************************************************* 
*@brief��  ����ָ����FTP URL������ftp��������ָ��Ŀ¼�µ������ļ�
*@param��  sFTP_URL: ����ָ����FTP URL                           :[in]
*@param��  dir_address: Ҫ������Ŀ¼                             :[in]
*@param��  filelist�� �������������ļ���[FileInformation]�б�    :��out��
*@param��  filename�� Ҫ�������ļ���                             :��in��
*@Return�� ������,0 = �ɹ���-1=����:û���ҵ�.
*************************************************/
//����ָ����FTP URL��list �����ļ�
int listFilesOnFTPWithUrl( std::string sFTP_URL ,std::string dir_address ,std::vector<cftp::FileInformation> &fileList , std::string filename="" );
//---zhangmiao:end--[7/10/2012]--

//@author zhangmiao@oristartech.com
//@date 2014-10-08
//@brief �����µ�ʱ��������룬sDestTZ='+08:00'Ϊ����ʱ�䣬��:sSrcTimeStr="2009-07-04T10:00:00+09:00"��ת��ΪsDestTimeStr="2009-07-04 09:00:00"��
//@new
int ConvertTimeZoneInfo( std::string sSrcTimeStr , std::string sDestTZ , std::string& sDestTimeStr );
//@modify(ʱ������)end;

#endif
