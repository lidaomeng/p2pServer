//@file:C_ErrorDef.h
//@brief: ������������Ϣ��Ϣ���塣
//@author:luyan@oristartech.com
//dade:2014-09-12

#ifndef _IMONITOR_ERROR_DEFINE
#define _IMONITOR_ERROR_DEFINE

typedef  const  int  c_u_int;

c_u_int INFO_IMonitor_START_RUN = 0x0001;


//Thread Manage Module: 1, subModule: 0000
//ERROR
//�̶߳����������޿����̡߳�
c_u_int ERROR_THREAD_LIST_FULL= 0x0001;
// ���Ի��߳��������ݴ���
c_u_int ERROR_INIT_THREAD_ATTRIB = 0x0003;
// �����߳��������ݴ���
c_u_int ERROR_SET_THREAD_ATTRIB = 0x0004;
// �����̴߳���
c_u_int ERROR_CREATE_TRREAD = 0x0005;
// �߳�״̬����
c_u_int ERROR_THREAD_STATE = 0x0006;
//����threadIDû���ҵ���Ӧ��C_ThreadData��
c_u_int ERROR_NO_FIND_THREADDATA_OF_ID = 0x0007;
// ���Ի�webservice�߳��������ݴ���
c_u_int ERROR_INIT_WEBSERVICE_THREAD_ATTRIB = 0x0008;
// ����webservice�߳��������ݴ���
c_u_int ERROR_SET_WEBSERVICE_THREAD_ATTRIB = 0x0009;
// ����webservice�̴߳���
c_u_int ERROR_WEBSERVICE_CREATE_TRREAD = 0x000a;
// �߳��Ѿ���ʱ��
c_u_int ERROR_THREAD_TIMEOUT = 0x000b;
// �̵߳�������û���ҵ�λִ����صĴ�������
c_u_int ERROR_THREAD_TASK_NUMBER_NO_FIND = 0x000d;
//�̵߳��������ʹ���
c_u_int ERROR_THREAD_TASK_TYPE = 0x000e;
// û�з����������߳�
c_u_int INFO_NO_CONDITION_THREAD = 02;
//��������������޿�������
c_u_int ERROR_TASK_LIST_FULL= 0x0001;



//Log Manage Module:7 subModule 0
//ERROR
//�ڴ���ʴ��� pLogManage û�б����Ի���
c_u_int ERROR_MEMORY_NOT_INIT = 0x0001;
//��ģ���ź���ģ���Ŷ�Ӧ����־��û���ҵ���
c_u_int ERROR_NOT_FIND_LOG_TYPE = 0x0002;
// ��ȡ��־��Ŀ¼���Դ���
c_u_int ERROR_GET_LOG_BOOT_PATH = 0x0003;
//������־Ŀ¼����
c_u_int ERROR_CREATE_LOG_PATH = 0x0004;
//��ȡĿ¼���Դ���
c_u_int ERROR_GET_LOG_PATH = 0x0005;
//ɾ�����ڵ���־����
c_u_int ERROR_DELETE_PRE_LOG = 0x0006;
//��־�������ô���
c_u_int ERROR_DELETE_LEVEL = 0x0007;  //
//��־ģ�������ô���
c_u_int ERROR_DELETE_MODULE = 0x0007;  //
//��־ģ�������ô���
c_u_int ERROR_DELETE_SUB_MODULE = 0x0008;  //



//Database Manage Module: 3, subModule: 0000;
//error
//��ѯ���Ϊ�ա�
const int ERROR_QUERY_RESULT_EMPTY = 0x0001;
//���¼�¼ʧ�ܡ�
const int ERROR_UPDATE_TABLE = 0x0002;
//ɾ�����ݼ�¼����
const int ERROR_DELETE_TABLE = 0x0003;
//�������ݼ�¼���� 
const int ERROR_INSERT_TABLE = 0x0004;
// �����ݿ����
const int ERROR_OPEN_DATABASE = 0x0005;   //db log;
//��ѯ���ݿ����
const int ERROR_QUERY_TABLE = 0x0006;  //db log;
//ִ��sql������
const int ERROR_EXEC_TABLE = 0x0007; //db log;
//��ʼ�������
const int ERROR_START_TRANSACTION = 0x0008; //db log;
//�ύ�������
const int ERROR_COMMIT_TRANSACTION = 0x0009; //db log;
//�ع��������
const int ERROR_ROLL_BACK_TRANSACTION = 0x000a; //db log;
//��ȡ�ֶ�ֵ����
const int ERROR_GET_FIELD = 0x000b;
const int ERROR_FIELD_VALUE_NULL_OR_ERROR = 0x000c;

//Thread Manage Module: 5, subModule: 0000
//ERROR
// MonitorSensor 
c_u_int ERROR_PARSE_MONITORSTATE_XML = 0x0001;

c_u_int ERROR_CREATE_HTTP = -1;
c_u_int ERROR_SENSOR_TCP_CONNECT = -2;
c_u_int ERROR_SENSOR_TCP_SEND = -3;
c_u_int ERROR_SENSOR_TCP_RECV = -4;
c_u_int ERROR_SIGCATCH_FUN = 0x0001;
// �߳��Ѿ���ʱ��
c_u_int ERROR_DEVSTATUS_FAULT = 0x0011;
const int ERROR_PLAYER_AQ_BADHTTPRESPONSE = -1;
const int ERROR_PLAYER_AQ_NEEDSOAPELEM = -2;
c_u_int ERROR_PLAYER_AQ_TCPCONNECT = -5;

//sms error
// ���ϲ��Ա�����
c_u_int ERROR_OTHERMONITOR_NORUN  = 0x0001;
c_u_int ERROR_SMSBUSY_NOTSWITCH = 0x0002;
c_u_int ERROR_SMSSWITCH_START = 0x0003;
c_u_int ERROR_SMSSWITCH_LOCALSHUTDOWN = 0x0004;
c_u_int ERROR_SMSSWITCH_CALLOTHERSW = 0x0005;
c_u_int ERROR_SMSSWITCH_LOCALRUN = 0x0006;
c_u_int ERROR_SMSSWITCH_LOCALRUNOK = 0x0007;
c_u_int ERROR_SMSSWITCH_LOCALRUNFAIL = 0x0008;

c_u_int ERROR_POLICY = 0x0010;
c_u_int ERROR_POLICYTRI_TMSSTARTUP = 0x0009;
c_u_int ERROR_POLICYTRI_SMSSWITCH = 0x000a;
c_u_int ERROR_POLICYTRI_ALLSMSSWITCH = 0x000b;
c_u_int ERROR_POLICYTRI_EXIT = 0x000c;
c_u_int ERROR_SMSBUSY_DELAYSWITCH = 0x000d;
c_u_int ERROR_READSMSTABLE_NOROW = 0x000e;
c_u_int ERROR_UPDATESMSTABLE_FAILED = 0x000f;
c_u_int ERROR_CALLOTHERWS_CONNFAIL = 0x0010;
c_u_int ERROR_GETSMSSTATUS_FAIL = 0x0011;

//Thread Manage Module: 4, subModule: 0000
//ERROR
// web���ʵ�ַ���ô��󣬷Ǳ�����ַ��
c_u_int ERROR_WEB_VISIT_ADDRESS = 0x0001;

//ERROR
const int SOCKET_WIN_ENVIRONMENT_ERROR   = 0x0001;   //Windows��������
const int SOCKET_NULL_FD                 = 0x0002;   //�յ�socket�ļ���������
const int SOCKET_CALL_SYSAPI_ERR         = 0x0003;	//����ϵͳAPI����
const int SOCKET_PARAR_ERR               = 0x0004;  	//�����������


//========SOCKET SERVER�˴�����==============================//
const int SERVER_SOCK_INI_ERR            = 0x1000;	//server��socket��ʼ������
const int SERVER_SOCK_BIND_ERR           = 0x1001;	//server��bind����
const int SERVER_SOCK_LISTEN_ERR         = 0x1002;	//server��listen����
const int SERVER_SOCK_ACCEPT_ERR         = 0x1003;	//server��accept����

//========SOCKET CLIENT�˴�����==============================//
const int CLIENT_SOCK_INI_ERR            = 0x2000;	//client��socket��ʼ������
const int CLIENT_SOCK_BIND_ERR           = 0x2001;	//client��bind����
const int CLIENT_SOCK_CONNECT_ERR        = 0x2003;	//client��connect����
const int CLIENT_SOCK_CONNECT_TIMEOUT    = 0x2004;	//client��connect��ʱ

//========SOCKET CONNECTION������==============================//
const int CONN_READ_PARA_ERR             = 0x3000;	//���Ӷ����������������
const int CONN_READ_CALL_ERR             = 0x3001;	//���Ӷ��������ô���
const int CONN_READ_SELECT_ERR           = 0x3002;	//���Ӷ�����select����
const int CONN_READ_TIMEOUT              = 0x3003;	//���Ӷ�������ʱ
const int CONN_READ_RECV_FAIL            = 0x3004;	//���Ӷ�����recv����
const int CONN_WRITE_PARA_ERR            = 0x3005;	//����д���������������
const int CONN_WRITE_CALL_ERR            = 0x3006;	//����д�������ô���
const int CONN_WRITE_SELECT_ERR          = 0x3007;	//����д����elect����
const int CONN_WRITE_TIMEOUT             = 0x3008;	//����д������ʱ
const int CONN_WRITE_SEND_FAIL           = 0x3009;	//����д����send����
const int CONN_PEER_TERMINATOR           = 0x300a;	//���ӶԶ˹ر�

#endif  
