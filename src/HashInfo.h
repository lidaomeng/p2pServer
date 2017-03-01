/*
*@file:HashInfo.h
*@brief:��ϣУ�飬�������⣬������Щ�޸ġ�
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __HASHINFO_H__
#define __HASHINFO_H__

#include <string>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "sha1.h"
#include "base64.h"

using namespace std;

#if 0
# define DIGEST_STREAM sha1_stream
# define DIGEST_BITS 160
# define DIGEST_REFERENCE "FIPS-180-1"
# define DIGEST_ALIGN 4

#define DIGEST_HEX_BYTES (DIGEST_BITS / 4)
#define DIGEST_BIN_BYTES (DIGEST_BITS / 8)

static inline void *
ptr_align (void const *ptr, size_t alignment)
{
  char const *p0 = (char const *)ptr;
  char const *p1 = p0 + alignment - 1;
  return (void *) (p1 - (size_t) p1 % alignment);
}
#endif

typedef struct hash_Records{
    string fileName;
    string uuid;
    string hash;
    unsigned long long fileSize;
}hash_Records_t; //used Hash Check when copy is finished by pthrf_cpdcp();

class HashInfo
{
public:
    HashInfo();
    ~HashInfo(){};

	//����stopCpFlag����Ϊ����ֵ�����ڷ���״ֵ̬��hashУ���Ƿ�ȡ��;
	//���hashУ�鱻ȡ�����򷵻�ֵ����=1�����򷵻�ֵ����=0��
    string getSha1FromFile(const char *file,char *bin_buffer,
                        unsigned long long *hasCpStop,int *stopCpFlag,int flag_cpucontl); 
    string toBase64(const char *bin_buffer);
private:
    bool digest_file(const char *filename, int *binary, char *bin_result,
        unsigned long long *hasCpSize,int *stopCpFlag,int flag_cpucontl);
private:
  //unsigned char bin_buffer_unaligned[DIGEST_BIN_BYTES + DIGEST_ALIGN];
public:
    
};


#endif

