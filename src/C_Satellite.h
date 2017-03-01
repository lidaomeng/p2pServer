//@file:C_Satellite.h
//@brief: ����Satellite�Ľӿڷ�����
//@author: zhangmiao@oristartech.com
//date: 2013-05-28
#ifndef _SAT_DCP_H_
#define _SAT_DCP_H_
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

//����TCPͨѶģʽ�����ض˿ڹ̶�Ϊ20080��
const unsigned short tcp_port = 20080 ;
const char device_ip[] = "192.168.0.7" ;
const int BUFFERLENGTH  = 65536 ;
const u8 nPreamble  = 0x55 ;         //0x55:��ͷ����ʼ��־
const u16 nCmd_Connect = 0x11 ;		     //0x11:��������
const int CheckSumLen = 4;	//У���4���ֽ�
const int CmdInfoHeaderLen = 7;		//ͷ��������payload��У���
#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
//dcp���սӿ�����ĸ�ʽ��ͷ�����ݽṹ
typedef struct CtrlCmdInfoHeader 
{
	unsigned char Preamble;	//0x55:��ͷ����ʼ��־
	u16 Cmd;
	u32 PayloadLength;	//Payload Length 4���ֽ�
	//u32 ChkSum;	//У���crc32
} CtrlCmdInfoHeader;
#pragma pack(pop)//�ָ�����״̬

//#include "FtpClient.h"
#include "C_TcpSocket.h"

namespace Satellite
{
	using namespace std;
	typedef struct DCPInfo
	{
		string PklUuid;
		string Name;
		string issuer;
		string issueDate;
		string Creater;
		DCPInfo(){}
	}DCPInfo;

	typedef struct FTP_DCPInfo
	{
		string asset_type;
		string source;
		string path;
		string username;
		string password;
		string ftp_url;
		string ftp_url_path;
		FTP_DCPInfo(){}
	}FTP_DCPInfo;
	
};

class C_Satellite 
{

	C_TcpSocket* m_tcp;

	std::string m_ip;		//IP��ַ
	unsigned short m_port;	//�˿ں�:20080

	std::string m_userName;
	std::string m_passwd;

	bool m_bConncetState;	//����״̬  0----�޷����ӣ�	1----��������

public:
	 C_Satellite(void);
	 ~C_Satellite(void);

	 //��ʼ�����������ǽ����豸tcp����
	 int Init(const std::string &strIp, unsigned short usPort = 20080	);

	 //�ر�������֤
	 int Close();

	 //����������֤����
	 int CreateCertConnect( );

	 //���ӰƬ��Ϣ�б�
	 //int GetDcpList( std::vector< std::string > &PklIdList );
	 int GetDcpList( std::vector< Satellite::DCPInfo > &DcpList );

	 //��ö�ӦӰƬFTP��Ϣ
	 int GetFTPDCPInfoById( const std::string &pklUuid , Satellite::FTP_DCPInfo& rFTP_DCPInfo );

	 //��ӦӰƬ������ϣ����status = 1����ʾӰƬ������ϡ�status = 0����ʾ����ʧ�ܡ�
	 int SetFTPDownLoadFinished(const std::string &pklUuid , const int status);

	 //�����֤����״̬
	 int GetCertConnect_Status();

private:
	//�����豸�Ŀ��������;rDatagramLenΪ���ɵı��ĳ���
	int SetCtrlCmd_Device( unsigned char* pBuffer , u16 nCmd, u32 PayloadLength, unsigned char* pPayload , int& rDatagramLen );

	int RecvInfo( char *buffer, int len , int& rHasRecvDataLen );

	std::string GetFTPDCPInfoByIdXml( const std::string &pklUuid );
	std::string FTPDownLoadFinishedAckXml( const std::string &pklUuid , const int status );

private:
	std::string m_xmlHeader;
	std::string m_responseBegin;
	std::string m_responseEnd;
};

#define ERROR_CONNECT_FAILED					-100	//������֤����ʧ��	
#define ERROR_CONNECT_ACK_FORMAT				-101	//����ȷ�ϸ�ʽ����
#define ERRPR_GET_DCPLIST_RESPONSE				-102	//ӰƬ��Ϣ�������ģ���Ӧ����
#define ERRPR_GETDCPLIST_ACK_FORMAT				-103	//ӰƬ��Ϣ�������ģ�����ʧ��
#define ERRPR_GET_FILM_FTPINFO_RESPONSE			-104	//��ӦӰƬFTP��Ϣ�������ģ���Ӧ����
#define ERRPR_GET_FILM_FTPINFO_ACK_FORMAT		-105	//��ӦӰƬFTP��Ϣ�������ģ�����ʧ��
#define ERROR_FTP_DOWNLOAD_FINISH_FAILED		-107	//ӰƬ������ϱ��ģ�����ʧ��
#define ERROR_FTP_DOWNLOAD_FINISH_ACK_FORMAT	-108	//ӰƬ������ϱ��ģ���Ӧ��ʽ����

#define ERROR_TCPTRANSPORTFAILED                -110    //TCP����ʧ��

#endif	//_SAT_DCP_H_
