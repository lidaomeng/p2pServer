#ifndef  COMMAND_H_
#define  COMMAND_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int Popen( const char* Command );
int system_Zip(const char * cmdstring, const char* cmdpath);

#endif //COMMAND_H_
