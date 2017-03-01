#include "rmDir.h"

//��������Ŀ¼�������ļ�
void remove_directory(const char *pDir)
{
	 // ������Ŀ¼��ָ��  
	DIR *dp;
	// ����dirent�ṹָ�뱣�����Ŀ¼  
	struct dirent *entry;
	// ����statbuf�ṹ�����ļ�����  
	struct stat statbuf;
	// ��Ŀ¼����ȡ��Ŀ¼��
	if((dp = opendir(pDir)) == NULL) 
	{  
		return;  
	}  
	// �л�����ǰĿ¼  
	chdir (pDir);
	// ��ȡ��һ��Ŀ¼��Ϣ�����δ����ѭ��  
	while((entry = readdir(dp)) != NULL) 
	{  
		// ��ȡ��һ����Ա����  
		lstat(entry->d_name, &statbuf);
		
		if(S_IFDIR &statbuf.st_mode)
		{  
			if ((strcmp(".", entry->d_name) == 0) 
			        || (strcmp("..", entry->d_name) == 0))  
			{
			        continue;
			}
			// �ݹ�ɾ��Ŀ¼
			remove_directory(entry->d_name);
		}  
		else
		{
			//ɾ��Ŀ¼�µ��ļ�
			remove(entry->d_name);
		}
	}
	
	chdir("..");
	//ɾ����Ŀ¼
	remove(pDir);
	
	closedir(dp);   
}
