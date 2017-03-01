#include "Popen.h"

int Popen( const char* Command ) 
{
	if( system( Command ) < 0 )
	{
		printf("system fork() failed!\n");
		return -1;
	}
		   		
	return 0;
}

int system_Zip(const char * cmdstring, const char* cmdpath)
{
	pid_t pid;
	int status;
	
	if(cmdstring == NULL)
	{
	    return (1); //���cmdstringΪ�գ����ط���ֵ��һ��Ϊ1
	}
	
	if((pid = fork())<0)
	{
	    status = -1; //forkʧ�ܣ�����-1
	}
	else if(pid == 0)
	{
			if(-1 == chdir(cmdpath) )
				_exit(127);
	    execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
	    _exit(127); // execִ��ʧ�ܷ���127��ע��execֻ��ʧ��ʱ�ŷ������ڵĽ��̣��ɹ��Ļ����ڵĽ��̾Ͳ�������~~
	}
	else //������
	{
	    while(waitpid(pid, &status, 0) < 0)
	    {
	        if(errno != EINTR)
	        {
	            status = -1; //���waitpid���ź��жϣ��򷵻�-1
	            break;
	        }
	    }
	}
	
	return status; //���waitpid�ɹ����򷵻��ӽ��̵ķ���״̬
}

