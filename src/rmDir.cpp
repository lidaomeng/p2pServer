#include "rmDir.h"

//解析工作目录下所有文件
void remove_directory(const char *pDir)
{
	 // 定义子目录流指针  
	DIR *dp;
	// 定义dirent结构指针保存后续目录  
	struct dirent *entry;
	// 定义statbuf结构保存文件属性  
	struct stat statbuf;
	// 打开目录，获取子目录流
	if((dp = opendir(pDir)) == NULL) 
	{  
		return;  
	}  
	// 切换到当前目录  
	chdir (pDir);
	// 获取下一级目录信息，如果未否则循环  
	while((entry = readdir(dp)) != NULL) 
	{  
		// 获取下一级成员属性  
		lstat(entry->d_name, &statbuf);
		
		if(S_IFDIR &statbuf.st_mode)
		{  
			if ((strcmp(".", entry->d_name) == 0) 
			        || (strcmp("..", entry->d_name) == 0))  
			{
			        continue;
			}
			// 递归删除目录
			remove_directory(entry->d_name);
		}  
		else
		{
			//删除目录下的文件
			remove(entry->d_name);
		}
	}
	
	chdir("..");
	//删除根目录
	remove(pDir);
	
	closedir(dp);   
}
