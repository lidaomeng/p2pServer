#ifndef _H_SOAPCLIENTPROCESS_
#define _H_SOAPCLIENTPROCESS_

#include "soapH.h"
#include <iostream>

//ip:�����webservice IP��ַ,port:�����webservice�˿ں�,arg0:����1,arg1:����2
//error:�쳣�ַ���,���������ط�0ʱ,���ַ�������˵���쳣ԭ��.
int TrnasferResult(const std::string &ip, unsigned short port, std::string &arg0, bool arg1, std::string &error);

#endif//_H_SOAPCLIENTPROCESS_
